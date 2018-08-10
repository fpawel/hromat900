object FormTestCalcStatDlg: TFormTestCalcStatDlg
  Left = 194
  Top = 132
  BorderStyle = bsDialog
  Caption = 'FormTestCalcStatDlg'
  ClientHeight = 372
  ClientWidth = 622
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 202
    Top = 358
    Width = 132
    Height = 13
    Caption = ' 31369-2008, 31371.7-2008'
  end
  object grdPhysChem: TStringGrid
    Left = 319
    Top = 7
    Width = 293
    Height = 325
    BorderStyle = bsNone
    Color = 14540253
    ColCount = 3
    DefaultColWidth = 10
    DefaultRowHeight = 20
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goEditing]
    TabOrder = 0
  end
  object Button1: TButton
    Left = 104
    Top = 338
    Width = 85
    Height = 27
    Caption = #1056#1072#1089#1095#1080#1090#1072#1090#1100
    TabOrder = 1
    OnClick = Button1Click
  end
  object grd1: TStringGrid
    Left = 7
    Top = 7
    Width = 306
    Height = 325
    BorderStyle = bsNone
    Color = 14540253
    ColCount = 3
    DefaultColWidth = 10
    DefaultRowHeight = 20
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goEditing]
    TabOrder = 2
  end
  object Button2: TButton
    Left = 7
    Top = 338
    Width = 85
    Height = 27
    Caption = #1042#1089#1090#1072#1074#1080#1090#1100
    TabOrder = 3
    OnClick = Button2Click
  end
  object CheckBox1: TCheckBox
    Left = 202
    Top = 338
    Width = 137
    Height = 14
    Caption = #1054#1082#1088#1091#1075#1083#1077#1085#1080#1077' '#1087#1086' '#1043#1054#1057#1058
    TabOrder = 4
  end
end
