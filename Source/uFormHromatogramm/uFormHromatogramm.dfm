object FormHromatogramm: TFormHromatogramm
  Left = 254
  Top = 206
  Width = 949
  Height = 723
  Caption = 'FormHromatogramm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  Icon.Data = {
    0000010001002020040000000000E80200001600000028000000200000004000
    0000010004000000000000020000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    0000000000000000000000000000000000000001110FFF011100000000000000
    0000000110FFFFF01100000000000000000000000F0000FF0100000000000000
    00000000F009900F01000000000000000000000F0999990F0100000000000000
    0000000F0990990F01000000000000000000000F0000990F0100000000000000
    0000FFF00000000F0110000000000000000FFFFF0000000F0010000000000000
    000FFFFFF000FFF00F00000000000000000FFFFFFFFFFFFF00F0000000000000
    000FFFFFFFFFFFF0FFFF000000000000000FFFF000FFFFFFFFFF000000000000
    000FFF00000FFFFFFFFFF000000000000000F0000000FFFFFFFFF00000000000
    000000FFF00000FFFFFFF00000000000000000FFFF00FFFFFFFFF00000000000
    000001000000000FFFFF0000000000000000010F000FFFF00000000000000000
    000000F0000F0FF01110000000000000000000F0000000FF0110000000000000
    0000010F000000FF01100000000000000000010FFF0F0FF01110000000000000
    00000110FF0FFFF0111100000000000000000010FF0FFF011110000000000000
    000000010FF0F011100000000000000000000000100101110000000000000000
    0000000001111111000000000000000000000000011111100000000000000000
    000000000001111000000000000000000000000000000000000000000000FC00
    001FFC00001FFC00001FFC00001FFC00000FFC00000FFC00000FFC00000FFC00
    000FFC00000FFC00000FFC00001FFC00001FFC00003FFC00003FFE00003FFE00
    003FFE00003FFE00003FFE00003FFE00003FFE00003FFF00003FFF00003FFF00
    007FFF8000FFFFC001FFFFE007FFFFF007FFFFF00FFFFFF80FFFFFFE1FFF}
  OldCreateOrder = False
  Position = poDefault
  Visible = True
  OnClose = FormClose
  PixelsPerInch = 120
  TextHeight = 16
  object PageControl1: TPageControl
    Left = 0
    Top = 41
    Width = 941
    Height = 650
    ActivePage = tbsGraph
    Align = alClient
    TabIndex = 0
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
        Width = 933
        Height = 619
        Align = alClient
        BorderStyle = bsNone
        Color = clBtnFace
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
        Width = 809
        Height = 619
        Align = alClient
        BorderStyle = bsNone
        Color = clBtnFace
        DefaultColWidth = 10
        DefaultRowHeight = 20
        RowCount = 1
        FixedRows = 0
        GridLineWidth = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
        TabOrder = 0
        OnMouseWheelDown = grdPikMouseWheelDown
        OnMouseWheelUp = grdPikMouseWheelUp
      end
      object PanelRight: TPanel
        Left = 809
        Top = 0
        Width = 124
        Height = 619
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
        Width = 933
        Height = 619
        Align = alClient
        BorderStyle = bsNone
        Color = clBtnFace
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
    Width = 941
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
      Width = 785
      Height = 41
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        785
        41)
      object cbHtgElem: TComboBox
        Left = 14
        Top = 7
        Width = 763
        Height = 24
        Style = csDropDownList
        Anchors = [akLeft, akTop, akRight]
        Color = 14540253
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
      Left = 904
      Top = 0
      Width = 37
      Height = 41
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 2
    end
  end
end
