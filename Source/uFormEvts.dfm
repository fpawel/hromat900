object FormEvts: TFormEvts
  Left = 194
  Top = 132
  Width = 870
  Height = 640
  Caption = #1061#1056#1054#1052#1040#1058'-900: '#1078#1091#1088#1085#1072#1083' '#1089#1086#1073#1099#1090#1080#1081
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  Visible = True
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object grdEvts: TStringGrid
    Left = 0
    Top = 0
    Width = 862
    Height = 606
    Align = alClient
    BorderStyle = bsNone
    DefaultRowHeight = 18
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing]
    TabOrder = 0
    OnDrawCell = grdEvtsDrawCell
    OnMouseDown = grdEvtsMouseDown
    ColWidths = (
      27
      64
      64
      64
      64)
  end
end
