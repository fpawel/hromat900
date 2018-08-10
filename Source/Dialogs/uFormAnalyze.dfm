object FormAnalyze: TFormAnalyze
  Left = 211
  Top = 171
  BorderStyle = bsDialog
  Caption = #1056#1072#1089#1095#1105#1090' '#1089#1088#1077#1076#1085#1080#1093' '#1087#1086#1082#1072#1079#1072#1090#1077#1083#1077#1081
  ClientHeight = 548
  ClientWidth = 972
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
  object Splitter1: TSplitter
    Left = 434
    Top = 33
    Width = 7
    Height = 515
    Cursor = crHSplit
    Color = cl3DLight
    ParentColor = False
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 972
    Height = 33
    Align = alTop
    Alignment = taLeftJustify
    BevelOuter = bvNone
    Color = clWindow
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object Label2: TLabel
      Left = 6
      Top = 7
      Width = 46
      Height = 16
      Cursor = crHandPoint
      Caption = 'Caption'
      DragCursor = crHandPoint
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -15
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
  end
  object grdPhyChe: TStringGrid
    Left = 441
    Top = 33
    Width = 531
    Height = 515
    Align = alClient
    BorderStyle = bsNone
    ColCount = 3
    DefaultColWidth = 100
    DefaultRowHeight = 20
    FixedCols = 0
    RowCount = 2
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing]
    TabOrder = 1
  end
  object grdConc: TStringGrid
    Left = 0
    Top = 33
    Width = 434
    Height = 515
    Align = alLeft
    BorderStyle = bsNone
    ColCount = 3
    DefaultColWidth = 100
    DefaultRowHeight = 20
    FixedCols = 0
    RowCount = 2
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
    TabOrder = 2
  end
end
