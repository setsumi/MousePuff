object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'MousePuff (Hide Mouse Pointer)'
  ClientHeight = 248
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  ShowHint = True
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 128
    Top = 13
    Width = 102
    Height = 13
    Caption = 'Hide after idling (sec)'
  end
  object radioBtnGlobal: TRadioButton
    Left = 12
    Top = 12
    Width = 105
    Height = 18
    Hint = 'Hide always'
    Caption = 'Global'
    Checked = True
    TabOrder = 0
    TabStop = True
    OnClick = radioBtnGlobalClick
  end
  object radioBtnProgram: TRadioButton
    Left = 12
    Top = 38
    Width = 105
    Height = 21
    Hint = 'Hide only when specific window is active'
    Caption = 'Program Specific'
    TabOrder = 1
    OnClick = radioBtnGlobalClick
  end
  object udTimeout: TUpDown
    Left = 285
    Top = 10
    Width = 16
    Height = 21
    Associate = editTimeout
    Min = 1
    Position = 3
    TabOrder = 3
  end
  object editTimeout: TEdit
    Left = 243
    Top = 10
    Width = 42
    Height = 21
    ReadOnly = True
    TabOrder = 2
    Text = '3'
  end
  object btnSpy: TButton
    Left = 301
    Top = 65
    Width = 53
    Height = 25
    Hint = 'Determine title and class of active window'
    Caption = 'Spy...'
    TabOrder = 7
    OnClick = btnSpyClick
  end
  object chkDebug: TCheckBox
    Left = 12
    Top = 223
    Width = 97
    Height = 17
    Caption = 'Debug sounds'
    TabOrder = 10
  end
  object btnHide: TButton
    Left = 300
    Top = 215
    Width = 53
    Height = 25
    Hint = 'Minimize to tray (Esc)'
    Caption = #8600' Hide '#8600
    TabOrder = 9
    OnClick = btnHideClick
  end
  object btnHelp: TButton
    Left = 315
    Top = 8
    Width = 26
    Height = 25
    Hint = 'Information'
    Caption = '?'
    TabOrder = 4
    OnClick = btnHelpClick
  end
  object btnExit: TButton
    Left = 301
    Top = 163
    Width = 53
    Height = 25
    Hint = 'Terminate program'
    Caption = #215' Exit '#215
    TabOrder = 8
    OnClick = btnExitClick
  end
  object chkStartToTray: TCheckBox
    Left = 151
    Top = 223
    Width = 134
    Height = 17
    Caption = 'Start minimized to tray'
    TabOrder = 11
  end
  object chkEnabled: TCheckBox
    Left = 133
    Top = 39
    Width = 124
    Height = 17
    Caption = 'Enabled (Ctrl+E)'
    Checked = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    State = cbChecked
    TabOrder = 5
    OnClick = chkEnabledClick
  end
  object pageControl1: TPageControl
    Left = 8
    Top = 65
    Width = 287
    Height = 152
    ActivePage = tabsheetClass
    TabOrder = 6
    object tabsheetTitle: TTabSheet
      Caption = 'Window Title(s)'
      object memoTitle: TMemo
        Left = 0
        Top = 0
        Width = 279
        Height = 124
        Align = alClient
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
    end
    object tabsheetClass: TTabSheet
      Caption = 'Window Class(es)'
      ImageIndex = 1
      object memoClass: TMemo
        Left = 0
        Top = 0
        Width = 279
        Height = 124
        Align = alClient
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
    end
  end
  object timerPuff: TTimer
    Enabled = False
    OnTimer = timerPuffTimer
    Left = 136
    Top = 124
  end
  object trayIcon: TTrayIcon
    Hint = 'MousePuff (Hide Mouse Pointer)'
    Icon.Data = {
      0000010001002020100000000000E80200001600000028000000200000004000
      0000010004000000000080020000000000000000000000000000000000000000
      000000008000008000000080800080000000800080008080000080808000C0C0
      C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF004444
      4444444444444444444444444444444444444444444444444444444444444444
      4444444444444444444444444444444444444444444444444444444444444444
      4444444444444444444444444444444444444444444884444444444444444444
      444444444448F4444444444444444444444444444448F4444444444444444444
      4444444444487444444444444444444444444444444884444444444444444444
      444444444448F7444444444444444444444444444447FF744444444444444444
      444444444444FFF844444444444444444444444444448FFF4444444444444444
      4444444444447FFF844444444444444444444444474448FFF444444444444444
      444444447F74448FF744444444444444444444448F844447F744444444444444
      44444448FFF8444484444444444444444444447F88FF84778444444444444444
      44444787448FFFF774444444444444444444477F8448FF874444444444444444
      44444478877477444444444444444444444444777F4444444444444444444444
      4444444788444444444444444444444444444444884444444444444444444444
      4444444478444444444444444444444444444444477444444444444444444444
      4444444447744444444444444444444444444444447444444444444444444444
      4444444444444444444444444444444444444444444444444444444444440000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000}
    Visible = True
    OnClick = trayIconClick
    Left = 92
    Top = 124
  end
end
