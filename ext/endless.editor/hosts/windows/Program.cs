/*
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

using Microsoft.Web.WebView2.Core;
using Microsoft.Web.WebView2.WinForms;

namespace EndlessControl.Windows;

internal static class Program
{
    [STAThread]
    private static void Main()
    {
        ApplicationConfiguration.Initialize();
        Application.Run(new MainForm());
    }
}

internal sealed class MainForm : Form
{
    private readonly WebView2 _webView = new() { Dock = DockStyle.Fill };

    public MainForm()
    {
        Text = "Endless Control";
        MinimumSize = new Size(720, 520);
        ClientSize = new Size(1120, 760);
        Controls.Add(_webView);
        Shown += LoadWebPageAsync;
    }

    private async void LoadWebPageAsync(object? sender, EventArgs e)
    {
        var pagePath = Path.Combine(AppContext.BaseDirectory, "web", "dist", "index.html");
        var developmentUrl = Environment.GetEnvironmentVariable("ENDLESS_CONTROL_WEB_URL");
        if (string.IsNullOrWhiteSpace(developmentUrl) && !File.Exists(pagePath))
        {
            MessageBox.Show(this, "Build the React app first with `npm --prefix web run build`.",
                "Web app not found", MessageBoxButtons.OK, MessageBoxIcon.Error);
            Close();
            return;
        }

        try
        {
            await _webView.EnsureCoreWebView2Async();
            if (!string.IsNullOrWhiteSpace(developmentUrl))
            {
                var uri = new Uri(developmentUrl, UriKind.Absolute);
                if (uri.Scheme != Uri.UriSchemeHttp && uri.Scheme != Uri.UriSchemeHttps)
                    throw new InvalidOperationException("ENDLESS_CONTROL_WEB_URL must be an HTTP or HTTPS URL.");
                _webView.CoreWebView2.Navigate(uri.AbsoluteUri);
                return;
            }
            _webView.CoreWebView2.SetVirtualHostNameToFolderMapping(
                "appassets.example",
                Path.GetDirectoryName(pagePath)!,
                CoreWebView2HostResourceAccessKind.DenyCors);
            _webView.CoreWebView2.Navigate("https://appassets.example/index.html");
        }
        catch (Exception ex)
        {
            MessageBox.Show(this, $"WebView2 could not start:\n\n{ex.Message}",
                "Unable to display the React app", MessageBoxButtons.OK, MessageBoxIcon.Error);
            Close();
        }
    }
}
