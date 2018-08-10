object FormHtgFilter: TFormHtgFilter
  Left = 328
  Top = 220
  BorderStyle = bsNone
  Caption = 'FormHtgFilter'
  ClientHeight = 164
  ClientWidth = 211
  Color = clWindow
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 211
    Height = 164
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = clWindow
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 16
      Width = 80
      Height = 18
      Cursor = crHandPoint
      Caption = #1050#1072#1085#1072#1083' '#1056#1043#1052
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      PopupMenu = PopupMenu2
      OnClick = Label2Click
    end
    object Label2: TLabel
      Left = 112
      Top = 16
      Width = 9
      Height = 18
      Cursor = crHandPoint
      Caption = '0'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsUnderline]
      ParentFont = False
      PopupMenu = PopupMenu2
      OnClick = Label2Click
    end
    object Label3: TLabel
      Left = 16
      Top = 136
      Width = 80
      Height = 18
      Cursor = crHandPoint
      Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      PopupMenu = PopupMenu2
      OnClick = Label3Click
    end
    object Label4: TLabel
      Left = 128
      Top = 136
      Width = 56
      Height = 18
      Cursor = crHandPoint
      Caption = #1054#1090#1084#1077#1085#1072
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      PopupMenu = PopupMenu2
      OnClick = Label4Click
    end
    object CheckBox1: TCheckBox
      Left = 16
      Top = 54
      Width = 153
      Height = 17
      Caption = #1048#1079#1084#1077#1088#1080#1090#1077#1083#1100#1085#1099#1077
      Checked = True
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      State = cbChecked
      TabOrder = 0
      OnClick = CheckBox1Click
    end
    object CheckBox2: TCheckBox
      Left = 16
      Top = 86
      Width = 153
      Height = 17
      Caption = #1043#1088#1072#1076#1091#1080#1088#1086#1074#1086#1095#1085#1099#1077
      Checked = True
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      State = cbChecked
      TabOrder = 1
      OnClick = CheckBox2Click
    end
  end
  object PopupMenu2: TPopupMenu
    AutoHotkeys = maManual
    AutoLineReduction = maManual
    Left = 136
    Top = 8
    object N1: TMenuItem
      Caption = ' '
      OnClick = N71Click
    end
    object N01: TMenuItem
      Caption = '0'
      OnClick = N71Click
    end
    object N11: TMenuItem
      Tag = 1
      Caption = '1'
      OnClick = N71Click
    end
    object N21: TMenuItem
      Tag = 2
      Caption = '2'
      OnClick = N71Click
    end
    object N31: TMenuItem
      Tag = 3
      Caption = '3'
      OnClick = N71Click
    end
    object N41: TMenuItem
      Tag = 4
      Caption = '4'
      OnClick = N71Click
    end
    object N51: TMenuItem
      Tag = 5
      Caption = '5'
      OnClick = N71Click
    end
    object N61: TMenuItem
      Tag = 6
      Caption = '6'
      OnClick = N71Click
    end
    object N71: TMenuItem
      Tag = 7
      Caption = '7'
      OnClick = N71Click
    end
  end
end
