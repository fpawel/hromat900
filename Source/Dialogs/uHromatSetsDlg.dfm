object FormHromatSetsDlg: TFormHromatSetsDlg
  Left = 197
  Top = 72
  Width = 1022
  Height = 837
  Caption = #1061#1056#1054#1052#1040#1058'-900: '#1085#1072#1089#1090#1088#1086#1081#1082#1072
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
  object Panel1: TPanel
    Left = 884
    Top = 0
    Width = 122
    Height = 799
    Align = alRight
    BevelOuter = bvNone
    Color = clWindow
    TabOrder = 0
    object Label1: TLabel
      Left = 0
      Top = 145
      Width = 122
      Height = 606
      Align = alClient
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      WordWrap = True
    end
    object Panel2: TPanel
      Left = 0
      Top = 0
      Width = 122
      Height = 145
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
      object Button1: TButton
        Left = 8
        Top = 8
        Width = 100
        Height = 25
        Caption = #1054#1090#1082#1088#1099#1090#1100'...'
        TabOrder = 0
        OnClick = Button1Click
      end
      object Button2: TButton
        Left = 8
        Top = 40
        Width = 100
        Height = 25
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100'...'
        TabOrder = 1
        OnClick = Button2Click
      end
      object btnRead: TButton
        Left = 8
        Top = 80
        Width = 100
        Height = 25
        Caption = #1057#1095#1080#1090#1072#1090#1100
        TabOrder = 2
      end
      object btnWrite: TButton
        Left = 8
        Top = 112
        Width = 100
        Height = 25
        Caption = #1047#1072#1087#1080#1089#1072#1090#1100
        TabOrder = 3
      end
    end
    object Panel3: TPanel
      Left = 0
      Top = 751
      Width = 122
      Height = 48
      Align = alBottom
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
    end
  end
  object ScrollBox1: TScrollBox
    Left = 0
    Top = 0
    Width = 884
    Height = 799
    Align = alClient
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    TabOrder = 1
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
      TabOrder = 0
      Visible = False
      OnExit = edGrdExit
      OnKeyDown = edGrdKeyDown
      OnKeyPress = edGrdKeyPress
    end
    object edTime: TDateTimePicker
      Left = 29
      Top = 220
      Width = 122
      Height = 21
      CalAlignment = dtaLeft
      Date = 40099
      Format = 'HH:mm'
      Time = 40099
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkTime
      ParseInput = False
      TabOrder = 1
      Visible = False
      OnKeyDown = edGrdKeyDown
    end
  end
end
