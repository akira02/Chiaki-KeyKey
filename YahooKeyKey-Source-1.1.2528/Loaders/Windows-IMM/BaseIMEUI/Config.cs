// [AUTO_HEADER]

using System;
using System.Collections.Generic;
using System.Text;
//using Takao;

namespace BaseIMEUI
{
    class Config
    {
        static string VersionForSearch = "2528";
        /// <summary>
        /// The URL of the Yahoo! Dictionary API
        /// This URL will redirect to
        /// http://w1.keykey.vip.tpc.yahoo.com/keykey/dict_gen_xmls.php?p=
        /// http://s1.dict.vip.tpe.yahoo.com/dict/xml/dict_gen_xmls.php?p=
        /// </summary>
        // public static string YahooDictionaryAPIURL = "http://w1.keykey.vip.tpc.yahoo.com/keykey/dict_gen_xmls.php?p=";
        public static string YahooDictionaryAPIURL = "http://tw.media.yahoo.com/keykey/dict_gen_xmls.php?p=";


        public static string YahooDictionaryBeaconURL
        {
            get
            {
                return "";
            //    Random random = new Random();                
            //    return "http://tw.rd.yahoo.com/referurl/keykey/client/onekey/dictionary/" + VersionForSearch + (Tracker.Is64BitMode() ? "W64" : "W32") + "/*http://tw.yimg.com/i/tw/download/ykkimg/blank.gif?rand=" + random.Next();             
            }
        }

		public static string YahooKeyKeyStartBeaconURL 
        {
            get
            {
                return "";
                //Random random = new Random();
                //return "http://tw.rd.yahoo.com/referurl/keykey/client/keykey/start/" + VersionForSearch + (Tracker.Is64BitMode() ? "W64" : "W32") + "/*http://tw.yimg.com/i/tw/download/ykkimg/blank.gif?rand=" + random.Next();
            }
        }
        public static string YahooDictionaryBeaconHTML = "<img src=\"" + YahooDictionaryBeaconURL + "\" style=\"display: none;\"/>";

        /// <summary>
        /// The URL of the Yahoo! Dictionary Website
        /// </summary>
        public static string YahooDictionaryURL = "http://tw.dictionary.yahoo.com";
        /// <summary>
        /// The text of the Yahoo! Dictionary copyright information.
        /// </summary>
        public static string YahooDictionaryCopyright = "<p class=\"credit\">\u96c5\u864e\u8cc7\u8a0a\u3000\u7248\u6b0a\u6240\u6709 \u00a9 2008-2010 Yahoo! Taiwan All Rights Reserved.</p>";
        public static string HelpURL = "http://tw.media.yahoo.com/keykey/help/";
    }
}
