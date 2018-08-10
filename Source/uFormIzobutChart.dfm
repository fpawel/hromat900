object FormIzobutChart: TFormIzobutChart
  Left = 415
  Top = 298
  Width = 693
  Height = 336
  Caption = #1043#1088#1072#1092#1080#1082' '#1082#1086#1085#1094#1077#1085#1090#1088#1072#1094#1080#1080' '#1080#1079#1086#1073#1091#1090#1080#1083#1077#1085#1072
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = False
  Position = poDefault
  Visible = True
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 677
    Height = 49
    Align = alTop
    Alignment = taLeftJustify
    BevelOuter = bvNone
    Caption = '      C"'#1080#1079#1086#1073#1091#1090#1080#1083#1077#1085'" =  ...'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 0
    Top = 49
    Width = 677
    Height = 24
    Align = alTop
    BevelOuter = bvNone
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Label1: TLabel
      Left = 0
      Top = 0
      Width = 105
      Height = 24
      Align = alLeft
      Caption = '   '#1042#1099#1087#1086#1083#1085#1077#1085#1086'   '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Segoe UI'
      Font.Style = []
      ParentFont = False
    end
    object CGauge1: TCGauge
      Left = 105
      Top = 0
      Width = 572
      Height = 24
      Align = alClient
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Segoe UI'
      Font.Style = []
      ForeColor = clNavy
      ParentFont = False
    end
  end
end
