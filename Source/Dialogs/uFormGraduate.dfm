object FormGraduate: TFormGraduate
  Left = 256
  Top = 203
  BorderStyle = bsDialog
  Caption = #1040#1088#1093#1080#1074' '#1075#1088#1072#1076#1091#1080#1088#1086#1074#1086#1082
  ClientHeight = 745
  ClientWidth = 672
  Color = clWindow
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object grd: TStringGrid
    Left = 0
    Top = 161
    Width = 672
    Height = 584
    Align = alClient
    BorderStyle = bsNone
    ColCount = 4
    DefaultColWidth = 130
    DefaultRowHeight = 25
    FixedColor = cl3DLight
    FixedCols = 0
    RowCount = 2
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
    ParentFont = False
    TabOrder = 0
    ColWidths = (
      130
      130
      130
      130)
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 672
    Height = 161
    Align = alTop
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object Label1: TLabel
      Left = 91
      Top = 13
      Width = 197
      Height = 21
      Caption = #1053#1086#1084#1077#1088' '#1093#1088#1086#1084#1072#1090#1086#1075#1088#1072#1084#1084#1099':'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 197
      Top = 104
      Width = 89
      Height = 21
      Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090':'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Label4: TLabel
      Left = 305
      Top = 13
      Width = 50
      Height = 21
      Caption = '12999'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clBlack
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Label6: TLabel
      Left = 84
      Top = 43
      Width = 206
      Height = 21
      Caption = #1054#1082#1086#1085#1095#1072#1085#1080#1077' '#1075#1088#1072#1076#1091#1080#1088#1086#1074#1082#1080':'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Label7: TLabel
      Left = 305
      Top = 43
      Width = 50
      Height = 21
      Caption = '12999'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clBlack
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Label9: TLabel
      Left = 7
      Top = 74
      Width = 285
      Height = 21
      Caption = #1053#1086#1084#1077#1088' '#1075#1088#1072#1076#1091#1080#1088#1086#1074#1086#1095#1085#1086#1075#1086' '#1073#1072#1083#1083#1086#1085#1072':'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clNavy
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Label10: TLabel
      Left = 305
      Top = 74
      Width = 50
      Height = 21
      Caption = '12999'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clBlack
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      ParentFont = False
    end
    object Memo1: TMemo
      Left = 305
      Top = 104
      Width = 336
      Height = 50
      BorderStyle = bsNone
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clRed
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = []
      Lines.Strings = (
        #1085#1077#1087#1088#1080#1077#1084#1083#1080#1084' ('#1086#1087#1086#1088#1085#1072#1103' '#1090#1072#1073#1083#1080#1094#1072' '#1085#1077' '
        #1089#1086#1086#1090#1074#1077#1090#1089#1090#1074#1091#1077#1090' '#1048#1055#1043')')
      ParentFont = False
      TabOrder = 0
    end
  end
end
