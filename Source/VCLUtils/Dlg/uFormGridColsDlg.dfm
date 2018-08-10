object FormGridColsDlg: TFormGridColsDlg
  Left = 200
  Top = 136
  Width = 413
  Height = 350
  BorderIcons = [biSystemMenu]
  Caption = #1042#1099#1073#1077#1088#1080#1090#1077' '#1074#1080#1076#1080#1084#1099#1077' '#1089#1090#1086#1083#1073#1094#1099' '#1090#1072#1073#1083#1080#1094#1099
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 120
  TextHeight = 16
  object clb: TCheckListBox
    Left = 0
    Top = 0
    Width = 395
    Height = 305
    OnClickCheck = clbClickCheck
    Align = alClient
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 25
    Items.Strings = (
      #1057#1090#1086#1083#1073#1077#1094'1'
      #1057#1090#1086#1083#1073#1077#1094'2'
      #1057#1090#1086#1083#1073#1077#1094'3'
      #1057#1090#1086#1083#1073#1077#1094'4'
      #1057#1090#1086#1083#1073#1077#1094'5'
      #1057#1090#1086#1083#1073#1077#1094'6'
      #1057#1090#1086#1083#1073#1077#1094'7')
    ParentColor = True
    ParentFont = False
    Style = lbOwnerDrawFixed
    TabOrder = 0
    OnKeyDown = clbKeyDown
  end
end
