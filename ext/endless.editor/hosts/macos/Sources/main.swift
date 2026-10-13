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

import AppKit
import WebKit

@main
final class AppDelegate: NSObject, NSApplicationDelegate {
    private var window: NSWindow?

    static func main() {
        let app = NSApplication.shared
        let delegate = AppDelegate()
        app.delegate = delegate
        app.setActivationPolicy(.regular)
        app.run()
    }

    func applicationDidFinishLaunching(_ notification: Notification) {
        guard let pageURL = resolvePageURL() else {
            let alert = NSAlert()
            alert.messageText = "React app not found"
            alert.informativeText = "Build it with `npm --prefix web run build` from the project root."
            alert.runModal()
            NSApp.terminate(nil)
            return
        }

        let webView = WKWebView(frame: .zero)
        if pageURL.isFileURL {
            webView.loadFileURL(pageURL, allowingReadAccessTo: pageURL.deletingLastPathComponent())
        } else {
            webView.load(URLRequest(url: pageURL))
        }

        let window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 1120, height: 760),
            styleMask: [.titled, .closable, .miniaturizable, .resizable],
            backing: .buffered,
            defer: false
        )
        window.title = "Endless Control"
        window.minSize = NSSize(width: 720, height: 520)
        window.center()
        window.contentView = webView
        window.makeKeyAndOrderFront(nil)
        self.window = window
        NSApp.activate(ignoringOtherApps: true)
    }

    private func resolvePageURL() -> URL? {
        if let developmentURL = ProcessInfo.processInfo.environment["ENDLESS_CONTROL_WEB_URL"],
           !developmentURL.isEmpty {
            guard let url = URL(string: developmentURL),
                  ["http", "https"].contains(url.scheme?.lowercased() ?? ""),
                  url.host != nil else { return nil }
            return url
        }
        let fileManager = FileManager.default
        var candidates: [URL] = []

        if let root = ProcessInfo.processInfo.environment["ENDLESS_CONTROL_WEB_ROOT"] {
            candidates.append(URL(fileURLWithPath: root, isDirectory: true).appendingPathComponent("index.html"))
        }
        candidates.append(URL(fileURLWithPath: fileManager.currentDirectoryPath)
            .appendingPathComponent("web/dist/index.html"))
        if let resources = Bundle.main.resourceURL {
            candidates.append(resources.appendingPathComponent("web/dist/index.html"))
        }

        return candidates.first { fileManager.fileExists(atPath: $0.path) }
    }
}
