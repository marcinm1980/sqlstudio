# Windows light and dark themes

The first implementation adds **System**, **Light**, and **Dark** to Preferences >
Fonts & Colors > Color Scheme. Existing Windows 7, alternative Windows 8, and
High Contrast schemes remain available. Saved numeric scheme IDs are preserved.

System reads the Windows **app** appearance preference (`AppsUseLightTheme`),
falling back to the previous light palette if that value cannot be read. An
explicit Light or Dark selection stays selected when Windows changes appearance.
Windows High Contrast takes precedence without replacing the saved selection.

The resolved palette is shared by the native drawing library, mforms, and the
managed Windows frontend. Appearance changes reuse `GNColorsChanged` to update
existing views, including the SQL editor's background, selection, caret, and
existing dark syntax styles. Initial coverage includes the home screen's existing
dark rendering, workspace tabs, sidebar text, status bar, and menu rendering.

Result grids, action output, and SQL history now share theme-aware cell, alternate
row, header, selection, and editing-control colors. Find/Replace fields, buttons,
and its options menu also refresh when the theme changes. These controls subscribe
while their window handles exist and unsubscribe when the handles are destroyed.
Changing colors does not rebuild grid columns or replace search text.

Model and plugin document fields now follow the same palette, including read-only
descriptions, selectors, and user datatype trees. The shared binding also covers
mforms fields, watches controls added later, and follows handle recreation and
disposal. Tree headers and unselected text use explicit palette colors; switching
back to Light restores native headers. The description tooltip is theme-aware,
and the main window reapplies its frame theme when its handle is created.

This is the theme foundation, not complete application-wide dark styling. Native
dialog controls, scrollbars, title bars, and
individual icons still need a visual audit and follow-up styling. macOS and Linux
appearance detection is unchanged.

## Validation

The `WindowsTheme.*` native tests check palette text contrast, editor background
and text, explicit selection persistence across refreshes, system preference
resolution, and compatibility of persisted IDs. They do not change Windows
settings and skip palette checks while Windows High Contrast is enabled.

After building the Debug Windows application, run
`testing/windows/RunThemeControlsSmoke.ps1`. This exercises real WinForms grid and
Find/Replace controls without showing application windows. It checks startup
colors, live switching, search text/selection and grid formatting preservation,
handle recreation, and disposal. It does not exercise database-backed cell edits
or replace a visual audit of populated results.

Manual checks for the Windows application:

1. Choose Dark, apply with OK, open an SQL workspace, and check text, selection,
   caret, tabs, sidebar, menus, and disabled menu items.
2. Choose Light and verify open views return to light colors. Restart and verify
   the selected preference is restored.
3. Choose System and change the Windows app theme while Studio remains open.
   Repeat with an explicit Light or Dark choice; that choice should stay active.
4. Enable Windows High Contrast, change its colors, then disable it. Verify the
   previous explicit or System choice is restored.
5. Load settings with a legacy scheme and verify it still selects the same scheme.
6. With results open, switch themes while editing a cell. Check its text, current
   selection, pending edits, alternate rows, row/column headers, NULL/BLOB icons,
   output/history grids, and Find/Replace fields.

Administration pages opened from the navigation tree now use the client palette
instead of forcing white. Existing pages refresh on GNColorsChanged. The Windows
mforms binding also themes boxes, tables, scroll containers, labels, group boxes,
and buttons, while preserving semantic text colors and custom canvas rendering.
This applies regardless of the managed server's operating system.

The control smoke test includes nested administration surfaces, late color
assignments, pages created in Dark then switched to Light, and the resolved System
palette. Run `python testing/windows/test_admin_theme.py` for the shared admin
page lifecycle checks; these require no running database.
