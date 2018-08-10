object FormTimeFilterDlg: TFormTimeFilterDlg
  Left = 522
  Top = 262
  BorderStyle = bsNone
  Caption = 'FormTimeFilterDlg'
  ClientHeight = 162
  ClientWidth = 281
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 281
    Height = 162
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = clWindow
    TabOrder = 0
    object Label2: TLabel
      Left = 7
      Top = 103
      Width = 12
      Height = 13
      Caption = #1076#1086
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label1: TLabel
      Left = 7
      Top = 47
      Width = 11
      Height = 13
      Caption = #1086#1090
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label3: TLabel
      Left = 184
      Top = 48
      Width = 80
      Height = 18
      Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      OnClick = Label3Click
    end
    object Label4: TLabel
      Left = 184
      Top = 88
      Width = 56
      Height = 18
      Caption = #1054#1090#1084#1077#1085#1072
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      OnClick = Label4Click
    end
    object Label5: TLabel
      Left = 184
      Top = 128
      Width = 47
      Height = 18
      Caption = #1057#1073#1088#1086#1089
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
      OnClick = Label5Click
    end
    object Label6: TLabel
      Left = 24
      Top = 8
      Width = 233
      Height = 18
      Caption = #1060#1080#1083#1100#1090#1088#1086#1074#1072#1090#1100' '#1087#1086' '#1076#1072#1090#1077' '#1080' '#1074#1088#1077#1084#1077#1085#1080
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object edTime2: TDateTimePicker
      Left = 26
      Top = 126
      Width = 131
      Height = 21
      CalAlignment = dtaLeft
      Date = 39839.6594328704
      Format = 'HH '#1095' m '#1084#1080#1085
      Time = 39839.6594328704
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkTime
      ParseInput = False
      TabOrder = 0
    end
    object edDate2: TDateTimePicker
      Left = 26
      Top = 100
      Width = 131
      Height = 21
      CalAlignment = dtaLeft
      Date = 39839.6594328704
      Format = 'dd MMM yyyy'
      Time = 39839.6594328704
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkDate
      ParseInput = False
      TabOrder = 1
    end
    object edTime1: TDateTimePicker
      Left = 26
      Top = 70
      Width = 131
      Height = 21
      CalAlignment = dtaLeft
      Date = 39839.6594328704
      Format = 'HH '#1095' m '#1084#1080#1085
      Time = 39839.6594328704
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkTime
      ParseInput = False
      TabOrder = 2
    end
    object edDate1: TDateTimePicker
      Left = 26
      Top = 44
      Width = 131
      Height = 21
      CalAlignment = dtaLeft
      Date = 39839.6594328704
      Format = 'dd MMM yyyy'
      Time = 39839.6594328704
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkDate
      ParseInput = False
      TabOrder = 3
    end
  end
end
