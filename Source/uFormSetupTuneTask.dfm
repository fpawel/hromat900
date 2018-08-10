object FormRele: TFormRele
  Left = 400
  Top = 270
  BorderStyle = bsNone
  Caption = 'FormRele'
  ClientHeight = 332
  ClientWidth = 414
  Color = clWindow
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 120
  TextHeight = 16
  object Panel4: TPanel
    Left = 10
    Top = 10
    Width = 395
    Height = 316
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Arial'
    Font.Style = []
    ParentColor = True
    ParentFont = False
    TabOrder = 0
    object CheckListBox1: TCheckListBox
      Left = 10
      Top = 10
      Width = 375
      Height = 225
      BorderStyle = bsNone
      Columns = 1
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'Arial'
      Font.Style = []
      IntegralHeight = True
      ItemHeight = 25
      Items.Strings = (
        #1042#1085#1077#1096#1085#1103#1103' '#1072#1074#1072#1088#1080#1103
        #1057'H '#8470'1: '#1087#1086#1088#1086#1075' 1/2'
        #1057'H '#8470'1: '#1082#1072#1083#1080#1073#1088#1086#1074#1082#1072' '#1085#1091#1083#1103' '
        #1057'H '#8470'1: '#1082#1072#1083#1080#1073#1088#1086#1074#1082#1072' '#1095#1091#1074#1089#1090#1074#1080#1090#1077#1083#1100#1085#1086#1089#1090#1080
        #1057'H '#8470'2: '#1087#1086#1088#1086#1075' 1/2'
        #1057'H '#8470'2: '#1082#1072#1083#1080#1073#1088#1086#1074#1082#1072' '#1085#1091#1083#1103' '
        #1057'H '#8470'2: '#1082#1072#1083#1080#1073#1088#1086#1074#1082#1072' '#1095#1091#1074#1089#1090#1074#1080#1090#1077#1083#1100#1085#1086#1089#1090#1080)
      ParentFont = False
      Style = lbOwnerDrawFixed
      TabOrder = 0
    end
    object Button2: TButton
      Left = 39
      Top = 266
      Width = 93
      Height = 31
      Caption = #1054#1082
      ModalResult = 1
      TabOrder = 1
    end
    object Button1: TButton
      Left = 148
      Top = 266
      Width = 92
      Height = 31
      Caption = #1054#1090#1084#1077#1085#1072
      ModalResult = 2
      TabOrder = 2
    end
  end
end
