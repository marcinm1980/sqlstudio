#!/usr/bin/env python3
"""
convert_casmine_to_gtest.py — Convert Casmine-style test patterns to Google Test.

Handles:
  $ModuleEnvironment() {};              → removed
  $TestData { ... };                    → struct <Suite>Data { ... };
  $describe("Name") {                  → namespace { class <FixtureName>Test : public ::testing::Test { ... }; }
  $beforeAll([...] { ... });           → static void SetUpTestSuite() { ... }
  $afterAll([...] { ... });            → static void TearDownTestSuite() { ... }
  $beforeEach([...] { ... });          → void SetUp() override { ... }
  $afterEach([...] { ... });           → void TearDown() override { ... }
  $it("name", [...] { ... });          → TEST_F(Fixture, Name) { ... }
  $expect(x).toBe(y)                   → EXPECT_EQ(x, y)
  $expect(x).toBe(y, msg)             → EXPECT_EQ(x, y) << msg
  $expect(x).toEqual(y)               → EXPECT_EQ(x, y)
  $expect(x).toEqual(y, msg)          → EXPECT_EQ(x, y) << msg
  $expect(x).toBeTrue(...)            → EXPECT_TRUE(x) [<< msg]
  $expect(x).toBeFalse(...)           → EXPECT_FALSE(x) [<< msg]
  $expect(x).Not.toBe(y, ...)         → EXPECT_NE(x, y)
  $expect(x).Not.toEqual(y, ...)      → EXPECT_NE(x, y)
  $expect(x).Not.toBeNull(...)        → EXPECT_NE(x, nullptr)
  $expect(x).Not.toBeValid(...)       → EXPECT_FALSE(x.is_valid())
  $expect(x).Not.toBeTrue(...)        → EXPECT_FALSE(x)
  $expect(x).toThrow(...)             → EXPECT_ANY_THROW(x)
  $expect(x).Not.toThrow(...)         → EXPECT_NO_THROW(x)
  $pending("msg");                     → GTEST_SKIP() << "msg";
  $fail("msg");                        → FAIL() << "msg";
"""

import re
import sys
import os
from pathlib import Path


def sanitize_test_name(name: str) -> str:
    """Convert a human-readable test name to a valid C++ identifier."""
    # Remove leading/trailing whitespace
    name = name.strip()
    # Replace common punctuation
    name = name.replace("::", "_")
    name = name.replace(".", "_")
    name = name.replace(",", "")
    name = name.replace(":", "")
    name = name.replace(";", "")
    name = name.replace("(", "")
    name = name.replace(")", "")
    name = name.replace("'", "")
    name = name.replace('"', "")
    name = name.replace("/", "_")
    name = name.replace("\\", "_")
    name = name.replace("*", "")
    name = name.replace("#", "Nr")
    name = name.replace("+", "Plus")
    name = name.replace("&", "And")
    name = name.replace("!", "Not")
    name = name.replace("?", "")
    name = name.replace("@", "At")
    name = name.replace("$", "")
    name = name.replace("%", "Pct")
    name = name.replace("^", "")
    name = name.replace("~", "")
    name = name.replace("`", "")
    name = name.replace("{", "")
    name = name.replace("}", "")
    name = name.replace("[", "")
    name = name.replace("]", "")
    name = name.replace("|", "")
    name = name.replace("<", "Lt")
    name = name.replace(">", "Gt")
    name = name.replace("=", "Eq")
    name = name.replace("-", "_")
    # Replace spaces with underscores, collapse multiple underscores
    name = re.sub(r'\s+', '_', name)
    name = re.sub(r'_+', '_', name)
    name = name.strip('_')
    # Ensure starts with a letter
    if name and not name[0].isalpha():
        name = "Test_" + name
    return name


def extract_describe_name(line: str) -> str:
    """Extract the name from $describe("...")."""
    m = re.search(r'\$describe\s*\(\s*"([^"]+)"\s*\)', line)
    if m:
        return m.group(1)
    return "UnnamedSuite"


def make_fixture_name(describe_name: str) -> str:
    """Convert $describe name to a C++ class name."""
    name = sanitize_test_name(describe_name)
    if not name.endswith("Test"):
        name += "Test"
    return name


def find_matching_brace(text: str, start: int) -> int:
    """Find the matching closing brace for the opening brace at position start."""
    depth = 0
    i = start
    in_string = False
    string_char = None
    escaped = False

    while i < len(text):
        c = text[i]

        if escaped:
            escaped = False
            i += 1
            continue

        if c == '\\':
            escaped = True
            i += 1
            continue

        if in_string:
            if c == string_char:
                in_string = False
            i += 1
            continue

        if c in ('"', "'"):
            in_string = True
            string_char = c
            i += 1
            continue

        # Handle line comments
        if c == '/' and i + 1 < len(text) and text[i + 1] == '/':
            # Skip to end of line
            nl = text.find('\n', i)
            if nl == -1:
                return -1
            i = nl + 1
            continue

        # Handle block comments
        if c == '/' and i + 1 < len(text) and text[i + 1] == '*':
            end_comment = text.find('*/', i + 2)
            if end_comment == -1:
                return -1
            i = end_comment + 2
            continue

        if c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                return i

        i += 1

    return -1


def find_paren_end(text: str, start: int) -> int:
    """Find the matching closing paren for the opening paren at position start."""
    depth = 0
    i = start
    in_string = False
    string_char = None
    escaped = False

    while i < len(text):
        c = text[i]

        if escaped:
            escaped = False
            i += 1
            continue

        if c == '\\':
            escaped = True
            i += 1
            continue

        if in_string:
            if c == string_char:
                in_string = False
            i += 1
            continue

        if c in ('"', "'"):
            in_string = True
            string_char = c
            i += 1
            continue

        if c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0:
                return i

        i += 1

    return -1


def convert_expect_simple(text: str) -> str:
    """Convert $expect(...) assertions to GTest macros, line by line."""

    lines = text.split('\n')
    result_lines = []

    for line in lines:
        converted = convert_expect_line(line)
        result_lines.append(converted)

    return '\n'.join(result_lines)


def convert_expect_line(line: str) -> str:
    """Convert a single line containing $expect to GTest."""

    # Handle $fail("msg");
    line = re.sub(
        r'\$fail\(\s*"([^"]*)"\s*\)',
        r'FAIL() << "\1"',
        line
    )

    # Handle $pending("msg");
    line = re.sub(
        r'\$pending\(\s*"([^"]*)"\s*\)',
        r'GTEST_SKIP() << "\1"',
        line
    )

    # We need to handle $expect() which may span multiple tokens.
    # Process all $expect patterns in the line.
    max_iterations = 20
    iteration = 0
    while '$expect(' in line and iteration < max_iterations:
        iteration += 1
        line = convert_one_expect(line)

    return line


def convert_one_expect(line: str) -> str:
    """Convert one $expect(...) pattern in the line."""

    idx = line.find('$expect(')
    if idx == -1:
        return line

    # Find the matching closing paren for $expect(
    paren_start = idx + len('$expect')
    paren_end = find_paren_end(line, paren_start)
    if paren_end == -1:
        return line  # Can't parse — leave as is

    actual_expr = line[paren_start + 1:paren_end].strip()

    rest = line[paren_end + 1:].lstrip()

    # Now parse the assertion chain
    # Possible patterns:
    # .Not.toBe(...)
    # .Not.toEqual(...)
    # .Not.toBeNull(...)
    # .Not.toBeValid(...)
    # .Not.toBeTrue(...)
    # .Not.toThrow(...)
    # .toBe(...)
    # .toEqual(...)
    # .toBeTrue(...)
    # .toBeFalse(...)
    # .toThrow(...)

    prefix = line[:idx]

    # .Not.toThrow(...)
    m = re.match(r'\.Not\.toThrow\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_NO_THROW({actual_expr}){suffix}"

    # .toThrow(...)
    m = re.match(r'\.toThrow\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_ANY_THROW({actual_expr}){suffix}"

    # .Not.toBeNull()  or .Not.toBeNull("msg")
    m = re.match(r'\.Not\.toBeNull\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_NE({actual_expr}, nullptr){suffix}"
    m = re.match(r'\.Not\.toBeNull\s*\(\s*"([^"]*)"\s*\)', rest)
    if m:
        msg = m.group(1)
        suffix = rest[m.end():]
        return f'{prefix}EXPECT_NE({actual_expr}, nullptr) << "{msg}"{suffix}'

    # .Not.toBeValid()
    m = re.match(r'\.Not\.toBeValid\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_FALSE({actual_expr}.is_valid()){suffix}"
    m = re.match(r'\.Not\.toBeValid\s*\(\s*"([^"]*)"\s*\)', rest)
    if m:
        msg = m.group(1)
        suffix = rest[m.end():]
        return f'{prefix}EXPECT_FALSE({actual_expr}.is_valid()) << "{msg}"{suffix}'

    # .Not.toBeTrue(...)
    m = re.match(r'\.Not\.toBeTrue\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_FALSE({actual_expr}){suffix}"
    m = re.match(r'\.Not\.toBeTrue\s*\(\s*"([^"]*)"\s*\)', rest)
    if m:
        msg = m.group(1)
        suffix = rest[m.end():]
        return f'{prefix}EXPECT_FALSE({actual_expr}) << "{msg}"{suffix}'

    # .Not.toBe(expected) or .Not.toBe(expected, "msg")
    m = re.match(r'\.Not\.toBe\s*\(', rest)
    if m:
        pstart = rest.find('(')
        pend = find_paren_end(rest, pstart)
        if pend != -1:
            args_str = rest[pstart + 1:pend].strip()
            suffix = rest[pend + 1:]
            args = split_top_level_args(args_str)
            if len(args) >= 2:
                expected = args[0].strip()
                msg = args[1].strip()
                return f'{prefix}EXPECT_NE({actual_expr}, {expected}) << {msg}{suffix}'
            else:
                expected = args[0].strip()
                return f"{prefix}EXPECT_NE({actual_expr}, {expected}){suffix}"

    # .Not.toEqual(expected) or .Not.toEqual(expected, "msg")
    m = re.match(r'\.Not\.toEqual\s*\(', rest)
    if m:
        pstart = rest.find('(')
        pend = find_paren_end(rest, pstart)
        if pend != -1:
            args_str = rest[pstart + 1:pend].strip()
            suffix = rest[pend + 1:]
            args = split_top_level_args(args_str)
            if len(args) >= 2:
                expected = args[0].strip()
                msg = args[1].strip()
                return f'{prefix}EXPECT_NE({actual_expr}, {expected}) << {msg}{suffix}'
            else:
                expected = args[0].strip()
                return f"{prefix}EXPECT_NE({actual_expr}, {expected}){suffix}"

    # .toBeTrue() or .toBeTrue("msg")
    m = re.match(r'\.toBeTrue\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_TRUE({actual_expr}){suffix}"
    m = re.match(r'\.toBeTrue\s*\(', rest)
    if m:
        pstart = rest.find('(')
        pend = find_paren_end(rest, pstart)
        if pend != -1:
            msg = rest[pstart + 1:pend].strip()
            suffix = rest[pend + 1:]
            if msg:
                return f"{prefix}EXPECT_TRUE({actual_expr}) << {msg}{suffix}"
            else:
                return f"{prefix}EXPECT_TRUE({actual_expr}){suffix}"

    # .toBeFalse() or .toBeFalse("msg")
    m = re.match(r'\.toBeFalse\s*\(\s*\)', rest)
    if m:
        suffix = rest[m.end():]
        return f"{prefix}EXPECT_FALSE({actual_expr}){suffix}"
    m = re.match(r'\.toBeFalse\s*\(', rest)
    if m:
        pstart = rest.find('(')
        pend = find_paren_end(rest, pstart)
        if pend != -1:
            msg = rest[pstart + 1:pend].strip()
            suffix = rest[pend + 1:]
            if msg:
                return f"{prefix}EXPECT_FALSE({actual_expr}) << {msg}{suffix}"
            else:
                return f"{prefix}EXPECT_FALSE({actual_expr}){suffix}"

    # .toBe(expected) or .toBe(expected, "msg")
    m = re.match(r'\.toBe\s*\(', rest)
    if m:
        pstart = rest.find('(')
        pend = find_paren_end(rest, pstart)
        if pend != -1:
            args_str = rest[pstart + 1:pend].strip()
            suffix = rest[pend + 1:]
            args = split_top_level_args(args_str)
            if len(args) >= 2:
                expected = args[0].strip()
                msg = args[1].strip()
                return f'{prefix}EXPECT_EQ({actual_expr}, {expected}) << {msg}{suffix}'
            else:
                expected = args[0].strip()
                return f"{prefix}EXPECT_EQ({actual_expr}, {expected}){suffix}"

    # .toEqual(expected) or .toEqual(expected, "msg")
    m = re.match(r'\.toEqual\s*\(', rest)
    if m:
        pstart = rest.find('(')
        pend = find_paren_end(rest, pstart)
        if pend != -1:
            args_str = rest[pstart + 1:pend].strip()
            suffix = rest[pend + 1:]
            args = split_top_level_args(args_str)
            if len(args) >= 2:
                expected = args[0].strip()
                msg = args[1].strip()
                return f'{prefix}EXPECT_EQ({actual_expr}, {expected}) << {msg}{suffix}'
            else:
                expected = args[0].strip()
                return f"{prefix}EXPECT_EQ({actual_expr}, {expected}){suffix}"

    # If no pattern matched, leave as is (will be caught in verification)
    return prefix + "$expect(" + actual_expr + ")" + rest


def split_top_level_args(s: str) -> list:
    """Split comma-separated arguments at top level (respecting parens, strings, templates)."""
    args = []
    depth_paren = 0
    depth_angle = 0
    depth_brace = 0
    in_string = False
    string_char = None
    escaped = False
    current = []

    for c in s:
        if escaped:
            current.append(c)
            escaped = False
            continue

        if c == '\\':
            escaped = True
            current.append(c)
            continue

        if in_string:
            current.append(c)
            if c == string_char:
                in_string = False
            continue

        if c in ('"', "'"):
            in_string = True
            string_char = c
            current.append(c)
            continue

        if c == '(':
            depth_paren += 1
        elif c == ')':
            depth_paren -= 1
        elif c == '<':
            depth_angle += 1
        elif c == '>':
            depth_angle -= 1
        elif c == '{':
            depth_brace += 1
        elif c == '}':
            depth_brace -= 1

        if c == ',' and depth_paren == 0 and depth_angle == 0 and depth_brace == 0:
            args.append(''.join(current))
            current = []
        else:
            current.append(c)

    if current:
        args.append(''.join(current))

    return args


def convert_file(filepath: str) -> str:
    """Read a file and convert all Casmine patterns to GTest."""

    with open(filepath, 'r') as f:
        text = f.read()

    original = text

    # Step 1: Remove $ModuleEnvironment() {};
    text = re.sub(r'\n?\s*\$ModuleEnvironment\s*\(\s*\)\s*\{\s*\}\s*;?\s*\n', '\n', text)

    # Step 2: Convert $expect assertions (do this early, before structural changes)
    text = convert_expect_simple(text)

    # Step 3: Convert $pending and $fail
    text = re.sub(r'\$pending\(\s*"([^"]*)"\s*\)', r'GTEST_SKIP() << "\1"', text)
    text = re.sub(r'\$fail\(\s*"([^"]*)"\s*\)', r'FAIL() << "\1"', text)

    # Step 4: Structural conversions — $TestData, $describe, $beforeAll, etc.
    # These require more careful handling and are done via multi-pass processing.

    # Convert $TestData { ... }; blocks
    text = convert_test_data(text, filepath)

    # Convert $describe blocks (the big one)
    text = convert_describe_blocks(text, filepath)

    if text != original:
        print(f"  [CONVERTED] {filepath}")
    else:
        print(f"  [NO CHANGE] {filepath}")

    return text


def convert_test_data(text: str, filepath: str) -> str:
    """Convert $TestData { ... }; to struct <Name>Data { ... };"""
    # Derive a name from the file
    basename = os.path.basename(filepath).replace('_specs.cpp', '').replace('.cpp', '')
    struct_name = ''.join(word.capitalize() for word in basename.split('_')) + "Data"

    pattern = r'\$TestData\s*\{'
    m = re.search(pattern, text)
    if not m:
        return text

    brace_start = text.index('{', m.start())
    brace_end = find_matching_brace(text, brace_start)
    if brace_end == -1:
        return text

    body = text[brace_start + 1:brace_end]

    # Find the semicolon after closing brace
    after_brace = text[brace_end + 1:].lstrip()
    semi_offset = 0
    if after_brace.startswith(';'):
        semi_offset = text.index(';', brace_end + 1) + 1
    else:
        semi_offset = brace_end + 1

    replacement = f"struct {struct_name} {{{body}}}"
    if not after_brace.startswith(';'):
        replacement += ';'

    text = text[:m.start()] + replacement + text[semi_offset:]
    return text


def convert_describe_blocks(text: str, filepath: str) -> str:
    """Convert $describe("Name") { ... } blocks to GTest fixture + TEST_F."""

    pattern = r'\$describe\s*\(\s*"([^"]+)"\s*\)\s*\{'
    m = re.search(pattern, text)
    if not m:
        return text

    describe_name = m.group(1)
    fixture_name = make_fixture_name(describe_name)

    brace_start = text.index('{', m.start() + len('$describe'))
    brace_end = find_matching_brace(text, brace_start)
    if brace_end == -1:
        print(f"  WARNING: Could not find matching brace for $describe in {filepath}")
        return text

    body = text[brace_start + 1:brace_end]

    # Check for $TestData struct name used in this file
    data_struct_match = re.search(r'struct\s+(\w+Data)\s*\{', text[:m.start()])
    data_struct_name = data_struct_match.group(1) if data_struct_match else None

    # Parse the body for $beforeAll, $afterAll, $beforeEach, $afterEach, and $it blocks
    fixture_members = []
    test_cases = []

    # Extract lifecycle blocks and test cases
    remaining_body = body

    # Extract $beforeAll
    before_all_body = extract_lifecycle_block(remaining_body, '$beforeAll')
    if before_all_body is not None:
        remaining_body = remove_lifecycle_block(remaining_body, '$beforeAll')

    after_all_body = extract_lifecycle_block(remaining_body, '$afterAll')
    if after_all_body is not None:
        remaining_body = remove_lifecycle_block(remaining_body, '$afterAll')

    before_each_body = extract_lifecycle_block(remaining_body, '$beforeEach')
    if before_each_body is not None:
        remaining_body = remove_lifecycle_block(remaining_body, '$beforeEach')

    after_each_body = extract_lifecycle_block(remaining_body, '$afterEach')
    if after_each_body is not None:
        remaining_body = remove_lifecycle_block(remaining_body, '$afterEach')

    # Extract $it blocks
    it_blocks = extract_it_blocks(remaining_body)

    # Build fixture class
    lines = []
    lines.append(f"class {fixture_name} : public ::testing::Test {{")
    lines.append("protected:")

    # Add static data pointer if $TestData was found
    if data_struct_name:
        lines.append(f"  static std::unique_ptr<{data_struct_name}> data;")
        lines.append("")

    if before_all_body is not None:
        setup_body = clean_lifecycle_body(before_all_body)
        lines.append("  static void SetUpTestSuite() {")
        if data_struct_name:
            lines.append(f"    data = std::make_unique<{data_struct_name}>();")
        lines.append(setup_body)
        lines.append("  }")
        lines.append("")
    elif data_struct_name:
        lines.append("  static void SetUpTestSuite() {")
        lines.append(f"    data = std::make_unique<{data_struct_name}>();")
        lines.append("  }")
        lines.append("")

    if after_all_body is not None:
        teardown_body = clean_lifecycle_body(after_all_body)
        lines.append("  static void TearDownTestSuite() {")
        lines.append(teardown_body)
        if data_struct_name:
            lines.append("    data.reset();")
        lines.append("  }")
        lines.append("")
    elif data_struct_name:
        lines.append("  static void TearDownTestSuite() {")
        lines.append("    data.reset();")
        lines.append("  }")
        lines.append("")

    if before_each_body is not None:
        setup_body = clean_lifecycle_body(before_each_body)
        lines.append("  void SetUp() override {")
        lines.append(setup_body)
        lines.append("  }")
        lines.append("")

    if after_each_body is not None:
        teardown_body = clean_lifecycle_body(after_each_body)
        lines.append("  void TearDown() override {")
        lines.append(teardown_body)
        lines.append("  }")
        lines.append("")

    lines.append("};")
    lines.append("")

    # Static member definition
    if data_struct_name:
        lines.append(f"std::unique_ptr<{data_struct_name}> {fixture_name}::data;")
        lines.append("")

    # Generate TEST_F for each $it block
    for test_name_raw, test_body in it_blocks:
        test_name = sanitize_test_name(test_name_raw)
        cleaned_body = clean_test_body(test_body)
        lines.append(f"TEST_F({fixture_name}, {test_name}) {{")
        lines.append(cleaned_body)
        lines.append("}")
        lines.append("")

    replacement = '\n'.join(lines)

    # Replace the $describe block (and trailing });)
    after_describe = text[brace_end + 1:].lstrip()
    # Remove trailing }); or } that closes the namespace or the $describe
    end_pos = brace_end + 1
    # Check for }\n} or just }
    remaining_after = text[brace_end + 1:]
    # Remove just the closing brace (we already parsed it)

    text = text[:m.start()] + replacement + text[end_pos:]

    return text


def extract_lifecycle_block(text: str, keyword: str) -> str:
    """Extract the body of a lifecycle block ($beforeAll, $afterAll, etc.)."""
    # Pattern: $beforeAll([&]() { ... });  or  $beforeAll([this]() { ... });
    # The lambda pattern varies: [&](), [this](), [&]{ ... }
    pattern = re.escape(keyword) + r'\s*\(\s*\[[\w&, ]*\]\s*\(\s*\)\s*\{'
    m = re.search(pattern, text)
    if not m:
        # Try without parens on lambda: $beforeAll([this] {
        pattern2 = re.escape(keyword) + r'\s*\(\s*\[[\w&, ]*\]\s*\{'
        m = re.search(pattern2, text)
        if not m:
            return None

    brace_start = text.index('{', m.start() + len(keyword))
    brace_end = find_matching_brace(text, brace_start)
    if brace_end == -1:
        return None

    return text[brace_start + 1:brace_end]


def remove_lifecycle_block(text: str, keyword: str) -> str:
    """Remove a lifecycle block from text."""
    pattern = re.escape(keyword) + r'\s*\(\s*\[[\w&, ]*\]\s*\(\s*\)\s*\{'
    m = re.search(pattern, text)
    if not m:
        pattern2 = re.escape(keyword) + r'\s*\(\s*\[[\w&, ]*\]\s*\{'
        m = re.search(pattern2, text)
        if not m:
            return text

    brace_start = text.index('{', m.start() + len(keyword))
    brace_end = find_matching_brace(text, brace_start)
    if brace_end == -1:
        return text

    # Find the closing ");  or  });"
    rest = text[brace_end + 1:]
    # Remove optional }); or });
    close_match = re.match(r'\s*\)\s*;?\s*', rest)
    if close_match:
        end_pos = brace_end + 1 + close_match.end()
    else:
        end_pos = brace_end + 1

    return text[:m.start()] + text[end_pos:]


def extract_it_blocks(text: str) -> list:
    """Extract all $it("name", [...]() { ... }); blocks."""
    blocks = []
    remaining = text
    max_iters = 200

    while max_iters > 0:
        max_iters -= 1
        # Find $it("...")
        m = re.search(r'\$it\s*\(\s*"([^"]+)"\s*,\s*\[[\w&, ]*\]\s*\(\s*\)\s*\{', remaining)
        if not m:
            # Try without parens: $it("...", [this] {
            m = re.search(r'\$it\s*\(\s*"([^"]+)"\s*,\s*\[[\w&, ]*\]\s*\{', remaining)
            if not m:
                break

        test_name = m.group(1)
        brace_start = remaining.index('{', m.start() + len('$it'))
        brace_end = find_matching_brace(remaining, brace_start)
        if brace_end == -1:
            break

        body = remaining[brace_start + 1:brace_end]
        blocks.append((test_name, body))

        # Skip past the });
        rest = remaining[brace_end + 1:]
        close_match = re.match(r'\s*\)\s*;?\s*', rest)
        if close_match:
            remaining = rest[close_match.end():]
        else:
            remaining = rest

    return blocks


def clean_lifecycle_body(body: str) -> str:
    """Clean a lifecycle body — adjust indentation, replace data-> refs."""
    lines = body.split('\n')
    # Remove leading/trailing empty lines
    while lines and not lines[0].strip():
        lines.pop(0)
    while lines and not lines[-1].strip():
        lines.pop()

    # Find minimum indentation
    min_indent = 9999
    for line in lines:
        if line.strip():
            indent = len(line) - len(line.lstrip())
            min_indent = min(min_indent, indent)

    if min_indent == 9999:
        min_indent = 0

    # Re-indent to 4 spaces
    result = []
    for line in lines:
        if line.strip():
            stripped = line[min_indent:]
            result.append('    ' + stripped)
        else:
            result.append('')

    return '\n'.join(result)


def clean_test_body(body: str) -> str:
    """Clean a test body — adjust indentation."""
    lines = body.split('\n')
    while lines and not lines[0].strip():
        lines.pop(0)
    while lines and not lines[-1].strip():
        lines.pop()

    min_indent = 9999
    for line in lines:
        if line.strip():
            indent = len(line) - len(line.lstrip())
            min_indent = min(min_indent, indent)

    if min_indent == 9999:
        min_indent = 0

    result = []
    for line in lines:
        if line.strip():
            stripped = line[min_indent:]
            result.append('  ' + stripped)
        else:
            result.append('')

    return '\n'.join(result)


def post_cleanup(text: str) -> str:
    """Final cleanup pass."""
    # Remove stray }); that were part of Casmine $describe or namespace closings
    # But be careful not to remove valid ones

    # Remove trailing empty namespace { } wrapper if present
    # Remove empty "namespace {" "}" wrappers around test code
    # (Casmine files sometimes wrap $describe in namespace {})

    # Clean up multiple blank lines
    text = re.sub(r'\n{4,}', '\n\n\n', text)

    # Remove any remaining $describe closing patterns like "}\n\n}"
    # This needs to be done carefully

    return text


def main():
    files = [
        "testing/test-suite/tests/modules/db.mysql/db_mysql_gen_grant_specs.cpp",
        "testing/test-suite/tests/modules/db.mysql/sql_create_specs.cpp",
        "testing/test-suite/tests/modules/db.mysql.parser/mysql_parser_module_specs.cpp",
        "testing/test-suite/tests/modules/db.mysql.parser/parse_datatypes_specs.cpp",
        "testing/test-suite/tests/modules/db.mysql.sqlparser/mysql_invalid_sql_parser_specs.cpp",
        "testing/test-suite/tests/modules/db.mysql.sqlparser/mysql_sql_facade_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/sqlide/wb_sql_editor_form_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/sqlide/wb_live_schema_tree_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_context_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_undo_editors_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_undo_others_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_model_file_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/ssh_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_copy_paste_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_lowlevel_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_module_specs.cpp",
        "testing/test-suite/tests/backend/wbprivate/studio/wb_undo_diagram_specs.cpp",
        "testing/test-suite/tests/backend/wbpublic/grt/grt_inspector_value_specs.cpp",
        "testing/test-suite/tests/backend/wbpublic/grtdb/editor_table_specs.cpp",
        "testing/test-suite/tests/backend/wbpublic/sqlide/sql_editor_be_autocomplete_specs.cpp",
        "testing/test-suite/tests/plugins/db.mysql/backend/db_mysql_plugin_specs.cpp",
        "testing/test-suite/tests/plugins/db.mysql/backend/model_diff_apply_specs.cpp",
        "testing/test-suite/tests/plugins/db.mysql.editors/backend/mysql_routinegroup_editor_specs.cpp",
    ]

    root = "/home/marcinm/develop/MySQLStudio"

    print("=" * 60)
    print("Casmine → GTest Converter")
    print("=" * 60)

    dry_run = "--dry-run" in sys.argv

    for rel_path in files:
        filepath = os.path.join(root, rel_path)
        if not os.path.exists(filepath):
            print(f"  [SKIP] {filepath} — file not found")
            continue

        converted = convert_file(filepath)
        converted = post_cleanup(converted)

        if not dry_run:
            with open(filepath, 'w') as f:
                f.write(converted)
            print(f"    Written: {filepath}")
        else:
            print(f"    [DRY RUN] Would write: {filepath}")

    print()
    print("Done. Run verification with:")
    print(f"  grep -rn '\\$expect\\|\\$describe\\|\\$it(\\|\\$TestData\\|\\$ModuleEnvironment\\|\\$beforeAll\\|\\$afterAll\\|\\$pending\\|\\$fail' {root}/testing/test-suite/tests/ --include='*.cpp'")


if __name__ == "__main__":
    main()
