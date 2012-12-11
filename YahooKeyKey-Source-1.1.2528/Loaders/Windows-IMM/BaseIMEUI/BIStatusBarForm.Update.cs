using System;
using System.Collections.Generic;
using System.Net;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Runtime;
using System.Runtime.InteropServices;
using Microsoft.Win32;
// using Takao;

namespace BaseIMEUI
{
    public partial class BIStatusBarForm
    {
        private DateTime m_lastCheckTime;
        private bool m_checkingUpdate = false;
        private bool m_lastFetchFailed = false;

		[DllImport("wininet.dll")]
		private extern static bool InternetGetConnectedState(out int Description, int ReservedValue);

        private bool InternetConnected()
        {
			int Desc;
			return InternetGetConnectedState(out Desc, 0);
			//if (NetworkInterface.GetIsNetworkAvailable() == false)
			//    return false;
			//try
			//{
			//    Dns.GetHostEntry("tw.yahoo.com");
			//    return true;
			//}
			//catch
			//{
			//    return false; // host not reachable. 
			//}
        }

        private void InstallServerTimer()
        {
            System.Windows.Forms.Timer serviceTimer = new System.Windows.Forms.Timer();
            serviceTimer.Tick += new EventHandler(TickServiceTimer);
            serviceTimer.Interval = 1000 * 60 * 10;
            serviceTimer.Enabled = true;
            serviceTimer.Start();
        }

        void TickServiceTimer(object sender, EventArgs e)
        {
            if (this.m_lastFetchFailed)
            {
                Utilities.msg("Tick service timer retry");
                this.FetchOnlineDataAndCheckForUpdate();
                return;
            }

            TimeSpan duration = DateTime.Now - this.m_lastCheckTime;
            if (duration.Hours > 24)
            {
                this.FetchOnlineDataAndCheckForUpdate();
                return;
            }
            // throw new Exception("The method or operation is not implemented.");
        }

        private void FetchOnlineDataAndCheckForUpdateInThread()
        {
            if (this.m_checkingUpdate)
                return;

            this.m_checkingUpdate = true;
            this.m_lastFetchFailed = false;

            if (!this.InternetConnected())
            {
                this.m_checkingUpdate = false;
                this.m_lastFetchFailed = true;
                return;
            }
            this.SendBeacon();
            this.CheckUpdate();

            this.m_lastCheckTime = DateTime.Now;

            //Adds more fetch actions.

            this.m_checkingUpdate = false;
            this.m_lastFetchFailed = false;
        }

        private void FetchOnlineDataAndCheckForUpdate()
        {
            ThreadStart threadStart = new ThreadStart(FetchOnlineDataAndCheckForUpdateInThread);
            Thread thread = new Thread(threadStart);
            thread.Start();
            
            BIServerConnector callback = BIServerConnector.SharedInstance;
            if (callback != null) {
                callback.startUpdateServerDataThread();
            }            
        }

        #region Auto update

        private void SendBeacon()
        {
            //try
            //{
            //    Tracker.SendTrackingURL(Config.YahooKeyKeyStartBeaconURL);
            //}
            //catch { }
        }

        private delegate void ConfirmDownloadUpdateCall();
        private void ConfirmDownloadUpdate()
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new ConfirmDownloadUpdateCall(ConfirmDownloadUpdate));
                return;
            }
            
            TakaoPreference.PanelUpdate.LaunchDownloadUpdateAppMute();
        }

        private void CheckUpdate()
        {
            string versionInfo = TakaoPreference.UpdateHelper.GetVersionInfo();
            string signature = TakaoPreference.UpdateHelper.GetVersionInfoSignature();
            if (versionInfo.Length == 0 || signature.Length == 0)
                return;
            bool valid = TakaoPreference.UpdateHelper.ValidateFile(versionInfo, signature);

            if (valid == false)
                return;

            if (TakaoPreference.UpdateHelper.ShouldUpdate(versionInfo) == true)
            {
                this.m_checkingUpdate = false;

                #region Check if user want to check for updates on launch.
                // Check if user want to check for updates on launch.
                // This information is stored in the preference plist file.
                BIServerConnector callback = BIServerConnector.SharedInstance;
                if (callback == null)
                    return;
                if (callback.hasLoaderConfigKey("ShouldCheckUpdateOnLaunch") == false)
                    callback.setLoaderConfigKeyStringValue("ShouldCheckUpdateOnLaunch", "true");
                if (callback.isLoaderConfigKeyTrue("ShouldCheckUpdateOnLaunch") == false)
                    return;
                #endregion

                #region Check if user ever checked "Do not notify me in a week".
                // Check if user ever checked "Do not notify me in a week".
                // This information is stored in Windows registry.

                RegistryKey registryKey = Registry.LocalMachine;
                registryKey = registryKey.OpenSubKey(@"SOFTWARE\Yahoo\KeyKey", true);

                if (registryKey != null)
                {
                    if (registryKey.GetValue("NextCheckDate") != null)
                    {
                        string timeString = registryKey.GetValue("NextCheckDate").ToString();
                        if (timeString.Length > 0)
                        {
                            DateTime nextCheckDate;
                            DateTime.TryParse(timeString, out nextCheckDate);

                            if (nextCheckDate < DateTime.Now.AddDays(7))
                                return;
                        }
                    }
                }
                #endregion

                this.ConfirmDownloadUpdate();
                return;
            }
        }


        #endregion

        /// <summary>
        /// Check if there is any newer version on the Internet on launch,
        /// </summary>
        private void CheckUpdateOnLaunch()
        {
            this.FetchOnlineDataAndCheckForUpdate();
            this.InstallServerTimer();
        }
    }
}
