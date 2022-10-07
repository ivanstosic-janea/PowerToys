﻿// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Automation.Peers;
using Microsoft.UI.Xaml.Automation.Provider;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Input;
using PowerToys.FileLocksmithUI.Properties;
using Windows.Graphics;
using WinUIEx;

namespace FileLocksmithUI
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : WindowEx, IDisposable
    {
        private string[] paths;

        public MainWindow()
        {
            paths = FileLocksmith.Interop.NativeMethods.ReadPathsFromFile();
            InitializeComponent();
            StartFindingProcesses();
        }

        private void OnRefreshClick(object sender, RoutedEventArgs e)
        {
            StartFindingProcesses();
        }

        private void StartFindingProcesses()
        {
            new Thread(FindProcesses).Start();
            DisplayProgressRing();
        }

        private void FindProcesses()
        {
            var result = FileLocksmith.Interop.NativeMethods.FindProcessesRecursive(paths);

            DispatcherQueue.TryEnqueue(() =>
            {
                stackPanel.Children.Clear();
                foreach (var item in result)
                {
                    var entry = new ProcessEntry(item.name, item.pid, (ulong)item.files.Length);

                    foreach (var path in item.files)
                    {
                        entry.AddPath(path);
                    }

                    stackPanel.Children.Add(entry);

                    // Launch a thread to erase this entry if the process exits
                    new Thread(() => WatchProcess(item.pid)).Start();
                }

                DisplayNoResultsIfEmpty();
            });
        }

        private void WatchProcess(uint pid)
        {
            if (FileLocksmith.Interop.NativeMethods.WaitForProcess(pid))
            {
                // This process has exited.
                DispatcherQueue.TryEnqueue(() =>
                {
                    for (int i = 0; i < stackPanel.Children.Count; i++)
                    {
                        var element = stackPanel.Children[i] as ProcessEntry;
                        if (element == null)
                        {
                            continue;
                        }

                        if (element.Pid == pid)
                        {
                            stackPanel.Children.RemoveAt(i);
                            DisplayNoResultsIfEmpty();
                            return;
                        }
                    }
                });
            }
        }

        private void DisplayNoResultsIfEmpty()
        {
            if (stackPanel.Children.Count == 0)
            {
                var textBlock = new TextBlock();

                textBlock.Text = PowerToys.FileLocksmithUI.Properties.Resources.NoResults;
                textBlock.FontSize = 24;
                textBlock.HorizontalAlignment = HorizontalAlignment.Center;
                textBlock.VerticalAlignment = VerticalAlignment.Center;

                stackPanel.Children.Add(textBlock);
            }
        }

        private void DisplayProgressRing()
        {
            stackPanel.Children.Clear();

            var ring = new ProgressRing();
            ring.Width = 64;
            ring.Height = 64;
            ring.Margin = new Thickness(0, 16, 0, 0);
            ring.IsIndeterminate = true;

            stackPanel.Children.Add(ring);
        }

        public void Dispose()
        {
        }
    }
}
