object FormCorruptedFilesMessageDialog: TFormCorruptedFilesMessageDialog
  Left = 394
  Top = 292
  BorderStyle = bsDialog
  Caption = #1055#1086#1074#1088#1077#1078#1076#1105#1085#1085#1099#1077' '#1092#1072#1081#1083#1099
  ClientHeight = 439
  ClientWidth = 641
  Color = clWindow
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  DesignSize = (
    641
    439)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 7
    Top = 10
    Width = 393
    Height = 13
    Caption = 
      #1057#1083#1077#1076#1091#1102#1097#1080#1077' '#1092#1072#1081#1083#1099' '#1093#1088#1086#1084#1072#1090#1086#1075#1088#1072#1084#1084' '#1087#1086#1074#1088#1077#1078#1076#1077#1085#1099' '#1080#1083#1080' '#1080#1084#1077#1102#1090' '#1085#1077#1074#1077#1088#1085#1099#1081' '#1092#1086#1088#1084#1072 +
      #1090
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object lv1: TListView
    Left = 7
    Top = 24
    Width = 626
    Height = 369
    Checkboxes = True
    Color = clWhite
    Columns = <
      item
        Caption = #8470
        Width = 70
      end
      item
        Caption = #1055#1091#1090#1100
        Width = 120
      end
      item
        Caption = #1060#1072#1081#1083
        Width = 120
      end
      item
        Caption = #1055#1088#1080#1095#1080#1085#1072
        Width = 300
      end>
    FlatScrollBars = True
    GridLines = True
    HotTrack = True
    HotTrackStyles = [htHandPoint]
    Items.Data = {
      BC0000000600000000000000FFFFFFFFFFFFFFFF020000000000000001310132
      013300000000FFFFFFFFFFFFFFFF020000000000000001320133013400000000
      FFFFFFFFFFFFFFFF020000000000000001330134013500000000FFFFFFFFFFFF
      FFFF020000000000000001340135013600000000FFFFFFFFFFFFFFFF02000000
      0000000001350136013700000000FFFFFFFFFFFFFFFF02000000000000000136
      01380139FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
    MultiSelect = True
    RowSelect = True
    ShowWorkAreas = True
    TabOrder = 0
    ViewStyle = vsReport
  end
  object Button2: TButton
    Left = 6
    Top = 404
    Width = 619
    Height = 27
    Anchors = [akLeft, akTop, akRight]
    Caption = #1059#1076#1072#1083#1080#1090#1100' '#1087#1086#1074#1088#1077#1078#1076#1105#1085#1085#1099#1077' '#1092#1072#1081#1083#1099
    TabOrder = 1
    OnClick = Button2Click
  end
end
