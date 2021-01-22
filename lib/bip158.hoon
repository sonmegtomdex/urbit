|%
+$  bits  [wid=@ dat=@ub]
++  params
  |%
  ++  p  19
  ++  m  784.931
  --
++  siphash
  |=  [k=byts m=byts]
  ^-  byts
  |^
  ?>  =(wid.k 16)
  ?>  (lte (met 3 dat.k) wid.k)
  ?>  (lte (met 3 dat.m) wid.m)
  =.  k  (flim:sha k)
  =.  m  (flim:sha m)
  (flim:sha (fin (comp m (init dat.k))))
  :: Initialise internal state
  ::
  ++  init
    |=  k=@
    ^-  [@ @ @ @]
    =/  k0=@  (end [6 1] k)
    =/  k1=@  (cut 6 [1 1] k)
    :^    (mix k0 0x736f.6d65.7073.6575)
        (mix k1 0x646f.7261.6e64.6f6d)
      (mix k0 0x6c79.6765.6e65.7261)
    (mix k1 0x7465.6462.7974.6573)
  ::
  :: Compression rounds
  ++  comp
    |=  [m=byts v=[v0=@ v1=@ v2=@ v3=@]]
    ^-  [@ @ @ @]
    =/  len=@ud  (div wid.m 8)
    =/  last=@  (lsh [3 7] (mod wid.m 256))
    =|  i=@ud
    =|  w=@
    |-
    =.  w  (cut 6 [i 1] dat.m)
    ?:  =(i len)
      =.  v3.v  (mix v3.v (mix last w))
      =.  v  (rnd (rnd v))
      =.  v0.v  (mix v0.v (mix last w))
      v
    %=  $
      v  =.  v3.v  (mix v3.v w)
         =.  v  (rnd (rnd v))
         =.  v0.v  (mix v0.v w)
         v
      i  (add i 1)
    ==
  ::
  :: Finalisation rounds
  ++  fin
    |=  v=[v0=@ v1=@ v2=@ v3=@]
    ^-  byts
    =.  v2.v  (mix v2.v 0xff)
    =.  v  (rnd (rnd (rnd (rnd v))))
    :-  8
    :(mix v0.v v1.v v2.v v3.v)
  ::
  :: Sipround
  ++  rnd
    |=  [v0=@ v1=@ v2=@ v3=@]
    ^-  [@ @ @ @]
    =.  v0  (~(sum fe 6) v0 v1)
    =.  v2  (~(sum fe 6) v2 v3)
    =.  v1  (~(rol fe 6) 0 13 v1)
    =.  v3  (~(rol fe 6) 0 16 v3)
    =.  v1  (mix v1 v0)
    =.  v3  (mix v3 v2)
    =.  v0  (~(rol fe 6) 0 32 v0)
    =.  v2  (~(sum fe 6) v2 v1)
    =.  v0  (~(sum fe 6) v0 v3)
    =.  v1  (~(rol fe 6) 0 17 v1)
    =.  v3  (~(rol fe 6) 0 21 v3)
    =.  v1  (mix v1 v2)
    =.  v3  (mix v3 v0)
    =.  v2  (~(rol fe 6) 0 32 v2)
    [v0 v1 v2 v3]
  --
::  +str: bit streams
::   read is from the front
::   write appends to the back
::
++  str
  |%
  ++  read-bit
    |=  s=bits
    ^-  [bit=@ub rest=bits]
    ?>  (gth wid.s 0)
    :*  ?:((gth wid.s (met 0 dat.s)) 0b0 0b1)
        [(dec wid.s) (end [0 (dec wid.s)] dat.s)]
    ==
  ::
  ++  read-bits
    |=  [n=@ s=bits]
    ^-  [bits rest=bits]
    =|  bs=bits
    |-
    ?:  =(n 0)  [bs s]
    =^  b  s  (read-bit s)
    $(n (dec n), bs (write-bits bs [1 b]))
  ::
  ++  write-bits
    |=  [s1=bits s2=bits]
    ^-  bits
    [(add wid.s1 wid.s2) (can 0 ~[s2 s1])]
  --
::  +gol: Golomb-Rice encoding/decoding
::
++  gol
  |%
  ::  +en: encode x and append to end of s
  ::   - s: bits stream
  ::   - x: number to add to the stream
  ::   - p: golomb-rice p param
  ::
  ++  en
    |=  [s=bits x=@ p=@]
    ^-  bits
    =+  q=(rsh [0 p] x)
    =+  unary=[+(q) (lsh [0 1] (dec (bex q)))]
    =+  r=[p (end [0 p] x)]
    %+  write-bits:str  s
    (write-bits:str unary r)
  ::
  ++  de
    |=  [s=bits p=@]
    ^-  [delta=@ rest=bits]
    |^  ?>  (gth wid.s 0)
    =^  q  s  (get-q s)
    =^  r  s  (read-bits:str p s)
    [(add dat.r (lsh [0 p] q)) s]
    ::
    ++  get-q
      |=  s=bits
      =|  q=@
      =^  first-bit  s  (read-bit:str s)
      |-
      ?:  =(0 first-bit)  [q s]
      =^  b  s  (read-bit:str s)
      $(first-bit b, q +(q))
    --
  --
--
