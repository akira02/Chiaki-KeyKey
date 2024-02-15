本倉庫乃奇摩輸入法廢件處理廠，目前維護下述 macOS 專用的奇摩輸入法元件：

- 奇摩輸入法偏好設定（ARM64 & AMD64），對應 target 名稱「Takao->Preferences」。
- 奇摩輸入法辭典編輯程式（ARM64 & AMD64），對應 target 名稱「Takao->PhraseEditor」。

如欲自行建置的話，請開啟「./YahooKeyKey-Source-1.1.2528/LithoglyphModules.xcodeproj」然後選擇上文提到的 TARGET 建置即可。
Xcode 可能會給出大量的 deprecation 警告，但有些是原理上很難無損修換成新版 API 的，請量力而行。

切記：這個倉庫是**廢件處理廠**、不是產品保固中心。**不會有誰活該對這些廢件的可用性負責**。

原始 README 資料：
```
Yahoo! KeyKey[1] is a customized Chinese input methods tool based on an open source project Openvanilla[2].

Yahoo! had decided to make Yahoo! Keykey as a open source projecet and under new BSD licene.

Some external dependency information are list here:
1.SinicaCorpus: You could reach http://www.sinica.edu.tw/ for the license.
2.BPMFMappings was created based on SinicaCorpus.
3.YahooSearchTerms: You could grabe hot keyword from http://tw.yahoo.com.
4.sqlite-cerod-see: Commerical SQLite Compressed and Encrypted Read-Only Database (CEROD) Extension, please check http://www.hwaci.com/sw/sqlite/cerod.html
5.DotMacKit.framework and PackageMaker.app: You can get it from Apple development kit.
6.CTGradient is licensed under http://creativecommons.org/licenses/by/2.5/ .

[1]. http://tw.media.yahoo.com/keykey/
[2]. http://openvanilla.org/
```
