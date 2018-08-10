object FormPikDlg: TFormPikDlg
  Left = 440
  Top = 129
  ActiveControl = cbGas
  BorderStyle = bsDialog
  Caption = 'FormPikDlg'
  ClientHeight = 123
  ClientWidth = 287
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 7
    Top = 2
    Width = 140
    Height = 13
    Caption = #1053#1072#1080#1084#1077#1085#1086#1074#1072#1085#1080#1077' '#1082#1086#1084#1087#1086#1085#1077#1085#1090#1072
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object LabeledEdit3: TLabeledEdit
    Left = 7
    Top = 98
    Width = 176
    Height = 21
    EditLabel.Width = 126
    EditLabel.Height = 13
    EditLabel.Caption = #1042#1088#1077#1084#1103' '#1091#1076#1077#1088#1078#1080#1074#1072#1085#1080#1103' t'#1091', c'
    EditLabel.Font.Charset = DEFAULT_CHARSET
    EditLabel.Font.Color = clNavy
    EditLabel.Font.Height = -11
    EditLabel.Font.Name = 'MS Sans Serif'
    EditLabel.Font.Style = []
    EditLabel.ParentFont = False
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 2
    OnExit = LabeledEdit3Exit
  end
  object Button1: TButton
    Left = 189
    Top = 7
    Width = 91
    Height = 33
    Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
    TabOrder = 3
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 189
    Top = 52
    Width = 91
    Height = 33
    Caption = #1054#1090#1084#1077#1085#1080#1090#1100
    TabOrder = 4
    OnClick = Button2Click
  end
  object LabeledEdit2: TLabeledEdit
    Left = 7
    Top = 59
    Width = 176
    Height = 21
    EditLabel.Width = 125
    EditLabel.Height = 13
    EditLabel.Caption = #1050#1086#1085#1094#1077#1085#1090#1088#1072#1094#1080#1103' C, '#1084#1086#1083#1100',%'
    EditLabel.Font.Charset = DEFAULT_CHARSET
    EditLabel.Font.Color = clNavy
    EditLabel.Font.Height = -11
    EditLabel.Font.Name = 'MS Sans Serif'
    EditLabel.Font.Style = []
    EditLabel.ParentFont = False
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 1
    OnExit = LabeledEdit3Exit
  end
  object cbGas: TComboBox
    Left = 7
    Top = 20
    Width = 176
    Height = 22
    Style = csOwnerDrawFixed
    DropDownCount = 1000
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 16
    ParentFont = False
    TabOrder = 0
  end
end
