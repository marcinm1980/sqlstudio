// Copyright (c) 2026 dev4fun. Licensed under GPL-2.0.
// Run with RunThemeControlsSmoke.ps1 after building the Windows application.
using System;
using System.Windows.Forms;
using MySQL;
using MySQL.Controls;
using MySQL.Utilities;

internal static class ThemeControlsSmoke
{
  private sealed class TestGrid : GridView
  {
    public TestGrid() : base(null) { }
    public void RecreateWindow() { RecreateHandle(); }
  }

  private sealed class TestTextBox : TextBox
  {
    public void RecreateWindow() { RecreateHandle(); }
  }

  private static void Check(bool condition, string message)
  {
    if (!condition) throw new Exception(message);
  }

  [STAThread]
  private static int Main()
  {
    try
    {
      if (SystemInformation.HighContrast)
        throw new Exception("Run the light/dark control checks with Windows High Contrast disabled.");
      Application.EnableVisualStyles();
      string logDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "theme-test-logs");
      System.IO.Directory.CreateDirectory(logDirectory);
      MySQL.MySqlStudio.Logger.InitLogger(logDirectory);
      Conversions.SetColorScheme(ColorScheme.ColorSchemeDark);
      using (var grid = new TestGrid())
      using (var find = new FindPanel())
      {
        grid.CreateControl();
        find.CreateControl();
        Check(grid.IsHandleCreated && find.IsHandleCreated, "Controls must subscribe to live theme notifications.");
        var search = (TextBox)find.Controls.Find("searchTextBox", true)[0];
        search.Text = "SELECT retained text";
        search.Select(7, 8);
        grid.DefaultCellStyle.WrapMode = DataGridViewTriState.True;
        grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
        Check(grid.BackgroundColor.GetBrightness() < 0.2f, "Dark empty grid background");
        Check(grid.DefaultCellStyle.ForeColor.GetBrightness() > 0.7f, "Dark cell text");
        Check(grid.AlternatingRowsDefaultCellStyle.BackColor.GetBrightness() < 0.2f, "Dark alternating rows");
        Check(!grid.EnableHeadersVisualStyles, "Dark headers must use the palette");
        Check(search.BackColor.GetBrightness() < 0.2f, "Dark search field");

        Conversions.SetColorScheme(ColorScheme.ColorSchemeStandardWin8);
        Check(grid.BackgroundColor == System.Drawing.SystemColors.Window, "Live light grid background");
        Check(grid.DefaultCellStyle.ForeColor == System.Drawing.SystemColors.WindowText, "Live light grid text");
        Check(grid.DefaultCellStyle.SelectionBackColor == System.Drawing.SystemColors.Highlight, "Light selection restored");
        Check(grid.EnableHeadersVisualStyles, "Light native headers restored");
        Check(search.BackColor == System.Drawing.SystemColors.Window, "Live light search field");

        grid.RecreateWindow();
        Conversions.SetColorScheme(ColorScheme.ColorSchemeDark);
        Check(grid.BackgroundColor.GetBrightness() < 0.2f, "Notifications survive handle recreation");
        Check(search.Text == "SELECT retained text" && search.SelectionStart == 7 && search.SelectionLength == 8,
          "Theme changes must preserve search text and selection");
        Check(grid.DefaultCellStyle.WrapMode == DataGridViewTriState.True, "Theme changes must preserve wrapping");
        Check(grid.ColumnHeadersDefaultCellStyle.Alignment == DataGridViewContentAlignment.MiddleRight,
          "Theme changes must preserve header alignment");
      }
      using (var document = new TabDocument())
      using (var text = new TestTextBox())
      using (var combo = new ComboBox())
      using (var tree = new Aga.Controls.Tree.TreeViewAdv())
      {
        document.Controls.Add(text);
        document.Controls.Add(combo);
        document.Controls.Add(tree);
        tree.Size = new System.Drawing.Size(300, 150);
        tree.UseColumns = true;
        tree.NodeControls.Add(new Aga.Controls.Tree.NodeControls.NodeTextBox { DataPropertyName = "Text" });
        var model = new Aga.Controls.Tree.TreeModel();
        model.Nodes.Add(new Aga.Controls.Tree.Node("VARCHAR"));
        tree.Model = model;
        tree.Columns.Add(new Aga.Controls.Tree.TreeColumn("Type", 100));
        tree.NodeControls[0].ParentColumn = tree.Columns[0];
        document.CreateControl();
        // Explicitly create handles: TabDocument is an embedded, initially hidden form.
        IntPtr textHandle = text.Handle, comboHandle = combo.Handle, treeHandle = tree.Handle;
        text.Text = "Uncommitted description";
        text.Select(3, 5);
        text.ReadOnly = true;
        combo.Items.Add("Selected object");
        combo.SelectedIndex = 0;
        Conversions.SetColorScheme(ColorScheme.ColorSchemeDark);
        Check(text.BackColor.GetBrightness() < 0.2f, "Dark read-only description");
        Check(combo.BackColor.GetBrightness() < 0.2f && combo.FlatStyle == FlatStyle.Flat, "Dark selector");
        Check(tree.BackColor.GetBrightness() < 0.2f && tree.HeaderBackColor.GetBrightness() < 0.2f, "Dark tree and header");
        using (var bitmap = new System.Drawing.Bitmap(tree.Width, tree.Height))
        {
          tree.DrawToBitmap(bitmap, tree.ClientRectangle);
          Check(bitmap.GetPixel(250, 5).GetBrightness() < 0.25f, "Rendered header remainder is dark");
          Check(bitmap.GetPixel(250, 100).GetBrightness() < 0.2f, "Rendered empty tree is dark");
          bitmap.Save(System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "theme-tree-dark.png"));
        }
        text.RecreateWindow();
        Check(text.BackColor.GetBrightness() < 0.2f, "Description theme survives handle recreation");
        using (var lateField = new TextBox())
        {
          document.Controls.Add(lateField);
          IntPtr lateHandle = lateField.Handle;
          Check(lateField.BackColor.GetBrightness() < 0.2f, "Late-added field is themed");
        }
        Conversions.SetColorScheme(ColorScheme.ColorSchemeStandardWin8);
        Check(text.BackColor == System.Drawing.SystemColors.Window, "Description restores light palette");
        Check(tree.HeaderBackColor.IsEmpty, "Tree restores native header rendering");
        Check(combo.FlatStyle == FlatStyle.Standard && combo.SelectedIndex == 0, "Selector style and selection restored");
        Check(text.Text == "Uncommitted description" && text.SelectionStart == 3 && text.SelectionLength == 5 && text.ReadOnly,
          "Description content, selection and read-only state survive switching");
      }
      using (var page = new Panel { BackColor = System.Drawing.Color.White, Size = new System.Drawing.Size(500, 300) })
      using (var table = new Panel { Dock = DockStyle.Fill })
      using (var label = new Label { Text = "Available Server Features", ForeColor = System.Drawing.Color.Black, AutoSize = true })
      using (var button = new Button { Text = "Refresh", Top = 40 })
      using (var status = new Label { Text = "Stopped", ForeColor = System.Drawing.Color.Red, Top = 80 })
      {
        page.Controls.Add(table);
        table.Controls.Add(label);
        table.Controls.Add(button);
        table.Controls.Add(status);
        foreach (Control control in new Control[] { page, table, label, button, status })
          ControlTheme.AttachSurface(control);
        page.CreateControl();
        Conversions.SetColorScheme(ColorScheme.ColorSchemeDark);
        Check(page.BackColor.GetBrightness() < 0.2f && table.BackColor.GetBrightness() < 0.2f, "Administration page and nested layout are dark");
        Check(label.ForeColor.GetBrightness() > 0.7f, "Administration label is readable");
        Check(button.BackColor.GetBrightness() < 0.2f && button.FlatStyle == FlatStyle.Flat, "Administration button is dark");
        Check(status.ForeColor == System.Drawing.Color.Red, "Semantic status color is preserved");
        page.BackColor = System.Drawing.Color.White;
        label.ForeColor = System.Drawing.Color.Black;
        Check(page.BackColor.GetBrightness() < 0.2f && label.ForeColor.GetBrightness() > 0.7f,
          "Late page refresh cannot restore white background or black text");
        using (var latePage = new Panel { BackColor = page.BackColor })
        {
          ControlTheme.AttachSurface(latePage);
          latePage.CreateControl();
          Conversions.SetColorScheme(ColorScheme.ColorSchemeStandardWin8);
          Check(latePage.BackColor.GetBrightness() > 0.8f, "Page created in dark mode returns to light");
        }
        Check(page.BackColor == System.Drawing.Color.White && label.ForeColor == System.Drawing.SystemColors.ControlText, "Administration restores light colors");
        Check(button.FlatStyle == FlatStyle.Standard, "Administration restores native button style");
        Conversions.SetColorScheme(ColorScheme.ColorSchemeStandard);
        Check((page.BackColor.GetBrightness() < 0.2f) == Conversions.InDarkMode(), "Administration follows resolved System palette");
        Conversions.SetColorScheme(ColorScheme.ColorSchemeDark);
        using (var bitmap = new System.Drawing.Bitmap(page.Width, page.Height))
        {
          page.DrawToBitmap(bitmap, page.ClientRectangle);
          Check(bitmap.GetPixel(450, 250).GetBrightness() < 0.2f, "Rendered administration layout is dark");
          bitmap.Save(System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "theme-admin-dark.png"));
        }
      }
      Conversions.SetColorScheme(ColorScheme.ColorSchemeDark);
      Conversions.SetColorScheme(ColorScheme.ColorSchemeStandardWin8);
      Console.WriteLine("PASS: administration surfaces, labels, buttons, System palette, late updates, grids, fields, rendering, state preservation, handle recreation, and disposal.");
      return 0;
    }
    catch (Exception error)
    {
      Console.Error.WriteLine(error);
      return 1;
    }
  }
}

