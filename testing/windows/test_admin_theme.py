"""Exercise real admin page lifecycle methods without a database or native UI."""
import ast
from pathlib import Path
from types import SimpleNamespace
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'plugins/wb.admin/frontend/wb_admin_main.py'


class View:
    def set_back_color(self, color):
        self.background = color


class Palette:
    background = '#1e1e1e'

    @classmethod
    def getSystemColor(cls, _):
        return SimpleNamespace(to_html=lambda: cls.background)


class AdminAppearanceTests(unittest.TestCase):
    def make_admin(self, platform):
        source = ast.parse(SOURCE.read_text())
        admin = next(node for node in source.body if isinstance(node, ast.ClassDef) and node.name == 'AdministratorTab')
        methods = [node for node in admin.body if isinstance(node, ast.FunctionDef) and node.name in ('add_page', 'updateColors')]
        namespace = {'sys': SimpleNamespace(platform=platform), 'Color': Palette, 'ControlBackgroundColor': 0}
        exec(compile(ast.Module(body=methods, type_ignores=[]), str(SOURCE), 'exec'), namespace)
        instance = View()
        instance.tabs = []
        instance.tabview = SimpleNamespace(add_page=lambda page, caption: None)
        # Appearance must never depend on the database server platform.
        instance.server_profile = SimpleNamespace(host_os='linux')
        for name in ('add_page', 'updateColors'):
            setattr(instance, name, namespace[name].__get__(instance))
        return instance

    def test_new_and_existing_pages_follow_client_palette(self):
        admin = self.make_admin('win32')
        Palette.background = '#1e1e1e'
        first, second = View(), View()
        admin.add_page(first)
        admin.add_page(second)
        self.assertEqual(first.background, Palette.background)
        Palette.background = '#ffffff'
        admin.updateColors(None, None, None)
        self.assertEqual([page.background for page in admin.tabs], ['#ffffff'] * 2)
        Palette.background = '#1e1e1e'
        admin.updateColors(None, None, None)
        self.assertEqual(first.background, Palette.background)

    def test_linux_pages_keep_native_theme(self):
        admin = self.make_admin('linux')
        page = View()
        admin.add_page(page)
        admin.updateColors(None, None, None)
        self.assertFalse(hasattr(page, 'background'))


if __name__ == '__main__':
    unittest.main()
