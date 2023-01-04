object FormMain: TFormMain
  Left = 506
  Top = 250
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Active Window Info Spy'
  ClientHeight = 124
  ClientWidth = 393
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  ShowHint = True
  PixelsPerInch = 96
  TextHeight = 13
  object memoInfo: TMemo
    Left = 0
    Top = 0
    Width = 393
    Height = 124
    Align = alClient
    BevelInner = bvLowered
    Lines.Strings = (
      'Activate the target window...')
    ScrollBars = ssBoth
    TabOrder = 0
    WordWrap = False
  end
  object StatusBar1: TStatusBar
    Left = 377
    Top = 105
    Width = 16
    Height = 19
    Align = alCustom
    Anchors = [akRight, akBottom]
    Panels = <>
    SimplePanel = True
  end
  object TimerPoll: TTimer
    Interval = 300
    OnTimer = TimerPollTimer
    Left = 80
    Top = 16
  end
end
