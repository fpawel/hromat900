object FormSoftIDDlg: TFormSoftIDDlg
  Left = 212
  Top = 126
  BorderStyle = bsDialog
  Caption = #1061#1056#1054#1052#1040#1058'-900: '#1080#1076#1077#1085#1090#1077#1092#1080#1082#1072#1090#1086#1088' '#1055#1054
  ClientHeight = 152
  ClientWidth = 433
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  PixelsPerInch = 120
  TextHeight = 16
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 417
    Height = 137
    BevelOuter = bvLowered
    TabOrder = 0
    object Label1: TLabel
      Left = 86
      Top = 60
      Width = 4
      Height = 16
      Caption = '-'
    end
    object Label2: TLabel
      Left = 166
      Top = 60
      Width = 4
      Height = 16
      Caption = '-'
    end
    object Label3: TLabel
      Left = 246
      Top = 60
      Width = 4
      Height = 16
      Caption = '-'
    end
    object Label4: TLabel
      Left = 16
      Top = 8
      Width = 186
      Height = 16
      Caption = #1042#1074#1077#1076#1080#1090#1077' '#1080#1076#1077#1085#1090#1080#1092#1080#1082#1072#1090#1086#1088' '#1055#1054
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label5: TLabel
      Left = 326
      Top = 60
      Width = 4
      Height = 16
      Caption = '-'
    end
    object Button1: TButton
      Left = 16
      Top = 96
      Width = 137
      Height = 33
      Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 160
      Top = 96
      Width = 137
      Height = 33
      Caption = #1054#1090#1084#1077#1085#1072
      TabOrder = 1
      OnClick = Button2Click
    end
    object Edit1: TEdit
      Left = 16
      Top = 56
      Width = 65
      Height = 24
      Color = cl3DLight
      MaxLength = 6
      ReadOnly = True
      TabOrder = 2
      Text = 'FFFFFF'
      OnExit = Edit1Exit
      OnKeyDown = Edit1KeyDown
    end
    object Edit2: TEdit
      Tag = 1
      Left = 96
      Top = 56
      Width = 65
      Height = 24
      MaxLength = 6
      TabOrder = 3
      OnExit = Edit1Exit
      OnKeyDown = Edit1KeyDown
    end
    object Edit3: TEdit
      Tag = 2
      Left = 176
      Top = 56
      Width = 65
      Height = 24
      MaxLength = 6
      TabOrder = 4
      OnExit = Edit1Exit
      OnKeyDown = Edit1KeyDown
    end
    object Edit4: TEdit
      Tag = 3
      Left = 256
      Top = 56
      Width = 65
      Height = 24
      MaxLength = 6
      TabOrder = 5
      OnExit = Edit1Exit
      OnKeyDown = Edit1KeyDown
    end
    object Edit5: TEdit
      Tag = 3
      Left = 336
      Top = 56
      Width = 65
      Height = 24
      MaxLength = 6
      TabOrder = 6
      OnExit = Edit1Exit
      OnKeyDown = Edit1KeyDown
    end
  end
end
