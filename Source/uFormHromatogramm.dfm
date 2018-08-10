object FormHromatogramm: TFormHromatogramm
  Left = 331
  Top = 210
  Width = 949
  Height = 565
  Caption = 'FormHromatogramm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  Icon.Data = {
    000001000200101010000000040028010000260000002020100000000400E802
    00004E0100002800000010000000200000000100040000000000C00000000000
    0000000000001000000000000000000000000000800000800000008080008000
    00008000800080800000C0C0C000808080000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00000000000000000000000000000000000000
    0000000000000000000000000000000000000000000007777777777000000777
    77777770800007917A2777708800077777777770880000000000000088000077
    7777777708000007777777777000000000000000000000000000000000000000
    0000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF800F
    FFFF0007FFFF0003FFFF0001FFFF0001FFFF0001FFFF8001FFFFC001FFFFE003
    FFFFFFFFFFFFFFFFFFFFFFFFFFFF280000002000000040000000010004000000
    0000800200000000000000000000100000000000000000000000000080000080
    000000808000800000008000800080800000C0C0C000808080000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000007777777777777777777
    7777700000000777777777777777777777777080000007777777777777777777
    7777708800000777777777777777777777777088800007777777777777777777
    7777708888000777917A27777777777777777088880007777777777777777777
    7777708888000777777777777777777777777088880000000000000000000000
    0000008888000077777777777777777777777708880000077777777777777777
    7777777088000000777777777777777777777777080000000777777777777777
    7777777770000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    00000000000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFFF
    FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8000003F0000
    001F0000000F0000000700000003000000010000000100000001000000010000
    000180000001C0000001E0000001F0000001F8000003FFFFFFFFFFFFFFFFFFFF
    FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
  OldCreateOrder = False
  Position = poDefault
  Visible = True
  OnClose = FormClose
  PixelsPerInch = 120
  TextHeight = 16
  object PageControl1: TPageControl
    Left = 0
    Top = 41
    Width = 931
    Height = 479
    ActivePage = TabSheet2
    Align = alClient
    TabIndex = 1
    TabOrder = 0
    object tbsGraph: TTabSheet
      Caption = #1043#1088#1072#1092#1080#1082
    end
    object TabSheet2: TTabSheet
      Caption = #1055#1080#1082#1080
      ImageIndex = 2
      object grdPik: TStringGrid
        Left = 0
        Top = 0
        Width = 923
        Height = 448
        Align = alClient
        BorderStyle = bsNone
        DefaultColWidth = 10
        DefaultRowHeight = 20
        RowCount = 1
        FixedRows = 0
        GridLineWidth = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
        OnMouseDown = grdPikMouseDown
        OnMouseWheelDown = grdPikMouseWheelDown
        OnMouseWheelUp = grdPikMouseWheelUp
      end
    end
    object TabSheet3: TTabSheet
      Caption = #1054#1087#1086#1088#1085#1072#1103' '#1090#1072#1073#1083#1080#1094#1072' '#1087#1080#1082#1086#1074
      ImageIndex = 2
      object grdBasePik: TStringGrid
        Left = 0
        Top = 0
        Width = 799
        Height = 448
        Align = alClient
        BorderStyle = bsNone
        DefaultColWidth = 10
        DefaultRowHeight = 20
        RowCount = 1
        FixedRows = 0
        GridLineWidth = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
        OnKeyDown = grdBasePikKeyDown
        OnMouseWheelDown = grdPikMouseWheelDown
        OnMouseWheelUp = grdPikMouseWheelUp
      end
      object PanelRight: TPanel
        Left = 799
        Top = 0
        Width = 124
        Height = 448
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 1
        object btnSaveBasePicTable: TButton
          Left = 9
          Top = 9
          Width = 112
          Height = 48
          Caption = #1047#1072#1087#1080#1089#1072#1090#1100
          TabOrder = 0
          OnClick = btnSaveBasePicTableClick
        end
        object Button2: TButton
          Left = 9
          Top = 64
          Width = 112
          Height = 49
          Caption = #1057#1095#1080#1090#1072#1090#1100
          TabOrder = 1
          OnClick = Button2Click
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = #1057#1087#1080#1089#1086#1082
      ImageIndex = 3
      object grdHtgsList: TStringGrid
        Left = 0
        Top = 0
        Width = 923
        Height = 606
        Align = alClient
        BorderStyle = bsNone
        ColCount = 1
        DefaultColWidth = 10
        DefaultRowHeight = 20
        FixedCols = 0
        RowCount = 1
        FixedRows = 0
        GridLineWidth = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
        OnDblClick = grdHtgsListDblClick
        OnMouseWheelDown = grdPikMouseWheelDown
        OnMouseWheelUp = grdPikMouseWheelUp
      end
    end
  end
  object pnDtctrElemInfo: TPanel
    Left = 0
    Top = 0
    Width = 931
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object lblDtctrElemInfo: TPanel
      Left = 0
      Top = 0
      Width = 119
      Height = 41
      Align = alLeft
      BevelOuter = bvNone
      Caption = 'lblDtctrElemInfo'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -15
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object Panel1: TPanel
      Left = 119
      Top = 0
      Width = 775
      Height = 41
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        775
        41)
      object cbHtgElem: TComboBox
        Left = 16
        Top = 7
        Width = 737
        Height = 24
        Style = csDropDownList
        Anchors = [akLeft, akTop, akRight]
        Color = clWhite
        DropDownCount = 15
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ItemHeight = 16
        ParentFont = False
        TabOrder = 0
        OnChange = cbHtgElemChange
      end
    end
    object Panel2: TPanel
      Left = 894
      Top = 0
      Width = 37
      Height = 41
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 2
    end
  end
  object Timer1: TTimer
    Interval = 30000
    OnTimer = Timer1Timer
    Left = 32
    Top = 88
  end
end
