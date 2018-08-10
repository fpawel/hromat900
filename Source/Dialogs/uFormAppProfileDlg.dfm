object FormAppProfileDlg: TFormAppProfileDlg
  Left = 309
  Top = 203
  Width = 414
  Height = 311
  BorderStyle = bsSizeToolWin
  Caption = #1061#1056#1054#1052#1040#1058'-900: '#1074#1099#1073#1086#1088' '#1087#1088#1086#1092#1080#1083#1103
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyDown = FormKeyDown
  PixelsPerInch = 120
  TextHeight = 16
  object Button1: TButton
    Left = 256
    Top = 9
    Width = 137
    Height = 32
    Caption = #1042#1099#1073#1088#1072#1090#1100
    Enabled = False
    ModalResult = 1
    TabOrder = 0
  end
  object ListBox1: TListBox
    Left = 8
    Top = 8
    Width = 233
    Height = 257
    ItemHeight = 16
    TabOrder = 1
    OnClick = ListBox1Click
    OnDblClick = ListBox1DblClick
    OnKeyDown = ListBox1KeyDown
  end
  object Button2: TButton
    Left = 256
    Top = 49
    Width = 137
    Height = 32
    Caption = #1044#1086#1073#1072#1074#1080#1090#1100
    TabOrder = 2
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 256
    Top = 233
    Width = 137
    Height = 32
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 7
    TabOrder = 3
  end
  object Button4: TButton
    Left = 256
    Top = 89
    Width = 137
    Height = 32
    Caption = #1059#1076#1072#1083#1080#1090#1100
    Enabled = False
    TabOrder = 4
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 256
    Top = 129
    Width = 137
    Height = 32
    Caption = #1055#1077#1088#1077#1080#1084#1077#1085#1086#1074#1072#1090#1100
    Enabled = False
    TabOrder = 5
    OnClick = Button5Click
  end
end
