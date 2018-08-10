object FormAppSetsDlg: TFormAppSetsDlg
  Left = 243
  Top = 125
  Width = 611
  Height = 551
  Caption = #1061#1056#1054#1052#1040#1058'-900: '#1087#1072#1088#1072#1084#1077#1090#1088#1099' '#1087#1088#1080#1083#1086#1078#1077#1085#1080#1103
  Color = clWindow
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object grd1: TStringGrid
    Left = 0
    Top = 0
    Width = 595
    Height = 463
    Align = alClient
    BorderStyle = bsNone
    ColCount = 3
    DefaultColWidth = 150
    DefaultRowHeight = 23
    FixedCols = 0
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goEditing]
    ParentFont = False
    TabOrder = 0
    OnSelectCell = grd1SelectCell
    OnSetEditText = grd1SetEditText
    ColWidths = (
      260
      100
      83)
  end
  object cbPortName: TComboBox
    Left = 23
    Top = 100
    Width = 150
    Height = 24
    Style = csOwnerDrawVariable
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 18
    ParentFont = False
    TabOrder = 1
    OnChange = rgInterfaceClick
    OnKeyDown = cbChanalKeyDown
  end
  object cbCommBR: TComboBox
    Left = 23
    Top = 139
    Width = 150
    Height = 25
    Style = csDropDownList
    DropDownCount = 100
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 17
    ParentFont = False
    TabOrder = 2
    OnChange = rgInterfaceClick
    OnKeyDown = cbChanalKeyDown
  end
  object cbDetectorType1: TComboBox
    Left = 23
    Top = 180
    Width = 146
    Height = 25
    DropDownCount = 100
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 17
    ParentFont = False
    TabOrder = 3
    OnChange = rgInterfaceClick
    OnKeyDown = cbChanalKeyDown
    Items.Strings = (
      #1044#1058#1055
      #1069#1061#1044
      #1055#1048#1044
      #1060#1048#1044
      '')
  end
  object edGrd: TEdit
    Left = 23
    Top = 260
    Width = 117
    Height = 29
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnExit = edGrdExit
    OnKeyDown = cbChanalKeyDown
  end
  object cbChanal: TComboBox
    Left = 23
    Top = 60
    Width = 150
    Height = 24
    Style = csOwnerDrawVariable
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 18
    ParentFont = False
    TabOrder = 5
    OnChange = rgInterfaceClick
    OnKeyDown = cbChanalKeyDown
    Items.Strings = (
      'TCP'
      'COM')
  end
  object edPath: TEdit
    Left = 23
    Top = 300
    Width = 117
    Height = 29
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    OnEnter = edPathEnter
    OnKeyDown = cbChanalKeyDown
  end
  object Panel1: TPanel
    Left = 0
    Top = 463
    Width = 595
    Height = 50
    Align = alBottom
    BevelOuter = bvNone
    Color = clWindow
    TabOrder = 7
    object Button2: TButton
      Left = 157
      Top = 8
      Width = 136
      Height = 33
      Caption = #1054#1090#1084#1077#1085#1072
      TabOrder = 0
      OnClick = Button2Click
    end
    object Button1: TButton
      Left = 7
      Top = 8
      Width = 137
      Height = 33
      Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
      TabOrder = 1
      OnClick = Button1Click
    end
  end
  object ComboBox1: TComboBox
    Left = 223
    Top = 212
    Width = 146
    Height = 25
    Style = csDropDownList
    DropDownCount = 100
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = []
    ItemHeight = 17
    ParentFont = False
    TabOrder = 8
    OnChange = rgInterfaceClick
    OnKeyDown = cbChanalKeyDown
    Items.Strings = (
      #1044#1072
      #1053#1077#1090)
  end
end
