::  Print what your agent is doing.
::
|=  [loud=? =agent:mall]
^-  agent:mall
|_  =bowl:mall
+*  this  .
    ag    ~(. agent bowl)
::
++  on-init
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-init" ~)
      .
  =^  cards  agent  on-init:ag
  [cards this]
::
++  on-save
  ^-  vase
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-save" ~)
      .
  on-save:ag
::
++  on-load
  |=  old-state=vase
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-load" ~)
      .
  =^  cards  agent  (on-load:ag old-state)
  [cards this]
::
++  on-poke
  |=  [=mark =vase]
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-poke with mark {<mark>}" ~)
      .
  ?:  ?=(%verb mark)
    `this(loud !loud)
  =^  cards  agent  (on-poke:ag mark vase)
  [cards this]
::
++  on-watch
  |=  =path
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-watch on path {<path>}" ~)
      .
  =^  cards  agent  (on-watch:ag path)
  [cards this]
::
++  on-leave
  |=  =path
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-leave on path {<path>}" ~)
      .
  =^  cards  agent  (on-leave:ag path)
  [cards this]
::
++  on-peek
  |=  =path
  ^-  (unit (unit cage))
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-peek on path {<path>}" ~)
      .
  (on-peek:ag path)
::
++  on-agent
  |=  [=wire =sign:agent:mall]
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-agent on wire {<wire>}, {<-.sign>}" ~)
      .
  =^  cards  agent  (on-agent:ag wire sign)
  [cards this]
::
++  on-arvo
  |=  [=wire =sign-arvo]
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  %+  slog
            leaf+"{<dap.bowl>}: on-arvo on wire {<wire>}, {<[- +<]:sign-arvo>}"
          ~
      .
  =^  cards  agent  (on-arvo:ag wire sign-arvo)
  [cards this]
::
++  on-fail
  |=  [=term =tang]
  ^-  (quip card:agent:mall agent:mall)
  =>  ?.  loud  .
      %-  (slog leaf+"{<dap.bowl>}: on-fail with term {<term>}" ~)
      .
  =^  cards  agent  (on-fail:ag term tang)
  [cards this]
--
