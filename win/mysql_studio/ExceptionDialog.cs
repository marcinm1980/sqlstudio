using System;
using System.Windows.Forms;
using MySqlStudio.X;

namespace MySqlStudio.X
{
  public partial class ExceptionDialog : Form
  {
    private static ExceptionDialog singleton = new ExceptionDialog();
    private String errorInfo;
    private ApplicationController controller;

    protected ExceptionDialog()
    {
      InitializeComponent();
    }

    public static void Show(String message, String info, ApplicationController controller)
    {
      singleton.messageLabel.Text = message;
      singleton.errorInfo = info;
      singleton.controller = controller;
      singleton.ShowDialog();
    }

    private void reportBugButton_Click(object sender, EventArgs e)
    {
      if (controller.isCommercial())
        System.Diagnostics.Process.Start("http://support.oracle.com");
      else
        //wbContext.report_bug(errorInfo);
        System.Diagnostics.Process.Start("http://bugs.mysql.com");
    }

    private void copyInfoButton_Click(object sender, EventArgs e)
    {
      Clipboard.SetText(errorInfo);
    }

    private void copyStackTraceToClipboardToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Clipboard.SetText(errorInfo);
    }

  }
}
