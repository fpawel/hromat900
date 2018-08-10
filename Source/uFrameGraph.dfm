object FrameGraph: TFrameGraph
  Left = 0
  Top = 0
  Width = 829
  Height = 674
  TabOrder = 0
  OnMouseWheelDown = FrameMouseWheelDown
  OnMouseWheelUp = FrameMouseWheelUp
  object Splitter1: TSplitter
    Left = 217
    Top = 0
    Width = 3
    Height = 674
    Cursor = crHSplit
    Color = clBtnFace
    ParentColor = False
  end
  object ScrollBox1: TScrollBox
    Left = 0
    Top = 0
    Width = 217
    Height = 674
    Align = alLeft
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    TabOrder = 0
    object grpBxY: TPanel
      Left = 0
      Top = 105
      Width = 217
      Height = 104
      Align = alTop
      BevelOuter = bvNone
      Color = clWindow
      TabOrder = 0
      DesignSize = (
        217
        104)
      object Label5: TLabel
        Left = 8
        Top = 8
        Width = 30
        Height = 16
        Caption = #1052#1080#1085'.'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label6: TLabel
        Left = 8
        Top = 56
        Width = 36
        Height = 16
        Caption = #1052#1072#1082#1089'.'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object edYMin: TEdit
        Left = 48
        Top = 6
        Width = 161
        Height = 24
        Anchors = [akLeft, akTop, akRight]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnChange = edYMinChange
      end
      object cbAutoYMin: TCheckBox
        Left = 48
        Top = 34
        Width = 55
        Height = 17
        Caption = #1040#1074#1090#1086
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = cbAutoYMinClick
      end
      object edYMax: TEdit
        Left = 48
        Top = 54
        Width = 161
        Height = 24
        Anchors = [akLeft, akTop, akRight]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        OnChange = edYMaxChange
      end
      object cbAutoYMax: TCheckBox
        Left = 48
        Top = 82
        Width = 55
        Height = 17
        Caption = #1040#1074#1090#1086
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        OnClick = cbAutoYMaxClick
      end
    end
    object grpBxX: TPanel
      Left = 0
      Top = 0
      Width = 217
      Height = 105
      Align = alTop
      BevelOuter = bvNone
      Color = clWindow
      TabOrder = 1
      DesignSize = (
        217
        105)
      object Label2: TLabel
        Left = 8
        Top = 8
        Width = 30
        Height = 16
        Caption = #1052#1080#1085'.'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label3: TLabel
        Left = 8
        Top = 56
        Width = 36
        Height = 16
        Caption = #1052#1072#1082#1089'.'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object edXMin: TEdit
        Left = 48
        Top = 6
        Width = 161
        Height = 24
        Anchors = [akLeft, akTop, akRight]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnChange = edXMinChange
        OnKeyDown = edXMinKeyDown
      end
      object cbAutoXMin: TCheckBox
        Left = 48
        Top = 34
        Width = 55
        Height = 17
        Caption = #1040#1074#1090#1086
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = cbAutoXMinClick
      end
      object edXMax: TEdit
        Left = 48
        Top = 54
        Width = 161
        Height = 24
        Anchors = [akLeft, akTop, akRight]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        OnChange = edXMaxChange
        OnKeyDown = edXMinKeyDown
      end
      object cbAutoXMax: TCheckBox
        Left = 48
        Top = 82
        Width = 55
        Height = 17
        Caption = #1040#1074#1090#1086
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        OnClick = cbAutoXMaxClick
      end
    end
  end
  object chrt_: TChart
    Left = 220
    Top = 0
    Width = 609
    Height = 674
    Cursor = crArrow
    BackWall.Brush.Color = clWhite
    BackWall.Color = clWhite
    BackWall.Pen.Width = 0
    BackWall.Pen.Visible = False
    Gradient.EndColor = 10526880
    Legend.Alignment = laBottom
    Legend.ColorWidth = 50
    Legend.DividingLines.Color = 8388863
    Legend.Font.Style = [fsBold]
    Legend.LegendStyle = lsSeries
    Legend.Shadow.Color = clWhite
    Legend.Shadow.HorizSize = 0
    Legend.Shadow.VertSize = 0
    Legend.Symbol.Width = 50
    Legend.TopPos = 0
    Legend.Visible = False
    MarginBottom = 0
    MarginLeft = 0
    MarginRight = 0
    MarginTop = 0
    Title.AdjustFrame = False
    Title.Alignment = taLeftJustify
    Title.Font.Color = clNavy
    Title.Font.Style = [fsBold, fsUnderline]
    Title.Text.Strings = (
      'dsfgsdfgwertmhgiuoui iykghjk')
    Title.Visible = False
    OnZoom = chrt_Zoom
    BottomAxis.Axis.Width = 0
    BottomAxis.DateTimeFormat = 'n:s'
    BottomAxis.LabelsSeparation = 20
    Frame.Width = 0
    Frame.Visible = False
    LeftAxis.Axis.Width = 0
    LeftAxis.AxisValuesFormat = '#0.#######'
    LeftAxis.ExactDateTime = False
    LeftAxis.Increment = 1E-5
    LeftAxis.Title.Angle = 0
    LeftAxis.Title.Font.Style = [fsBold]
    View3D = False
    Zoom.Pen.Color = clBlack
    Zoom.Pen.Width = 2
    OnAfterDraw = chrt_AfterDraw
    Align = alClient
    BevelOuter = bvNone
    Color = clWhite
    DockSite = True
    TabOrder = 1
    OnMouseDown = chrt_MouseDown
    OnMouseMove = chrt_MouseMove
    OnMouseUp = chrt_MouseUp
    OnResize = chrt_Resize
    OnMouseWheelDown = chrt_MouseWheelDown
    OnMouseWheelUp = chrt_MouseWheelUp
    object panelGraphTools: TPanel
      Left = 72
      Top = 48
      Width = 265
      Height = 105
      BevelOuter = bvNone
      Color = clWindow
      TabOrder = 0
      object cbAutoLimitY: TCheckBox
        Left = 8
        Top = 8
        Width = 240
        Height = 17
        Caption = #1040#1074#1090#1086#1087#1088#1086#1082#1088#1091#1090#1082#1072
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnClick = cbAutoLimitYClick
      end
      object cbNoZoom: TCheckBox
        Left = 8
        Top = 31
        Width = 240
        Height = 17
        Caption = #1048#1089#1093#1086#1076#1085#1099#1081' '#1088#1072#1079#1084#1077#1088
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = cbNoZoomClick
      end
      object cbShowPics: TCheckBox
        Left = 8
        Top = 55
        Width = 240
        Height = 17
        Caption = #1055#1080#1082#1080
        Checked = True
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 2
        OnClick = cbShowPicsClick
      end
      object cbCrossHair: TCheckBox
        Left = 8
        Top = 79
        Width = 240
        Height = 17
        Caption = #1050#1091#1088#1089#1086#1088
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        OnClick = cbCrossHairClick
      end
    end
    object panelView: TPanel
      Left = 72
      Top = 280
      Width = 265
      Height = 105
      BevelOuter = bvNone
      Color = clWindow
      TabOrder = 1
      object btnBrushColor: TLabel
        Left = 5
        Top = 8
        Width = 123
        Height = 16
        Cursor = crHandPoint
        Caption = #1062#1074#1077#1090' '#1092#1086#1085#1072' '#1096#1082#1072#1083#1099'...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = btnBrushColorClick
      end
      object Label4: TLabel
        Left = 5
        Top = 32
        Width = 84
        Height = 16
        Cursor = crHandPoint
        Caption = #1062#1074#1077#1090' '#1083#1080#1085#1080#1080'...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = Label4Click
      end
      object Label7: TLabel
        Left = 5
        Top = 57
        Width = 110
        Height = 16
        Cursor = crHandPoint
        Caption = #1058#1086#1083#1097#1080#1085#1072' '#1083#1080#1085#1080#1080'...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = Label7Click
      end
      object Label1: TLabel
        Left = 5
        Top = 81
        Width = 142
        Height = 16
        Cursor = crHandPoint
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1082#1072#1082' '#1090#1077#1082#1089#1090'...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = Label1Click
      end
    end
  end
  object ColorDialog1: TColorDialog
    Ctl3D = True
    Left = 232
    Top = 32
  end
end
