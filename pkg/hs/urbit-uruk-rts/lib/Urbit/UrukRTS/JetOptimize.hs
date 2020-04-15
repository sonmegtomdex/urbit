module Urbit.UrukRTS.JetOptimize where

import ClassyPrelude hiding (try, evaluate)
import System.IO.Unsafe

import Control.Arrow    ((>>>))
import Data.Function    ((&))
import Numeric.Natural  (Natural)
import Numeric.Positive (Positive)
import Prelude          ((!!))

import qualified GHC.Exts            as GHC
import qualified Urbit.UrukRTS.Types as F

--------------------------------------------------------------------------------

type Nat = Natural
type Pos = Positive

--------------------------------------------------------------------------------

data Node
    = VSeq
    | VS
    | VK
    | VIn Pos
    | VBn Pos
    | VCn Pos
    | VSn Pos
    | VIff
    | VCas
    | VLet
  deriving stock (Eq, Ord, Generic)

instance Show Node where
  show = \case
    VSeq  -> "Q"
    VS    -> "S"
    VK    -> "K"
    VIn 1 -> "I"
    VBn 1 -> "B"
    VCn 1 -> "C"
    VIn n -> "I" <> show n
    VSn n -> "S" <> show n
    VBn n -> "B" <> show n
    VCn n -> "C" <> show n
    VIff  -> "Iff"
    VCas  -> "Cas"
    VLet  -> "Let"

data Code = Code
    { cArgs :: Pos
    , cName :: F.Val
    , cBody :: F.Val
    , cFast :: Val
    , cLoop :: Bool
    }
  deriving stock (Eq, Ord, Generic)

syms = singleton <$> "xyzpqrstuvwxyzabcdefghijklmnop"

sym i | i >= length syms = "v" <> show i
sym i                    = syms !! i

instance Show Code where
    show c@(Code n nm _ v lop) =
        regr <> header (fromIntegral n) <> prettyVal v
      where
        arity ∷ Int
        arity = fromIntegral n

        regr = "~/  " <> show n <> "  " <> show nm <> "\n"

        header ∷ Int → String
        header 0 | lop = "..  $\n"
        header 0       = ""
        header n       = header (n-1) <> "|=  " <> sym (arity - n) <> "\n"

{- |
    There are three kinds of things

    - Functions that we know how to reduce.
    - Functions that we don't know how to reduce.
    - Functions that we want to turn into control flow.
    - Recursive references.
    - Stack references.

    `Kal` is a function that we don't want to reduce.

    `Clo` is a partially-saturated thing that we *do* know how to reduce.

    `Rec` and `Ref` are recursive calls and stack references.

    `App` is unevaluated function application.

    `Let`, `Cas`, and `Iff` are understood control flow.
-}
data Exp
    = Clo Int Node [Exp]
    | Kal F.Node [Exp]
    | Rec [Exp]
    | Ref Nat [Exp]
    | Reg Nat [Exp]
    | Iff Exp Exp Exp [Exp]
    | Cas Nat Exp Exp Exp [Exp]
    | Let Nat Exp Exp [Exp]
    | App Exp Exp
  deriving stock (Eq, Ord, Generic)

{- |
    A `Val` is the same as an expression except that it contains no
    `App` nodes (and no `Clo` nodes).

    - Everything has been evaluated as far as possible, and everything
      is in closure-form.

    - `Clo` nodes are eliminated because the distinction between `Kal`
      and `Clo` is only relevant during the reduction that happens in
      this module.
-}
data Val
    = ValKal F.Node [Val]
    | ValRec [Val]
    | ValRef Nat [Val]
    | ValReg Nat [Val]
    | ValIff Val Val Val [Val]
    | ValCas Nat Val Val Val [Val]
    | ValLet Nat Val Val [Val]
  deriving stock (Eq, Ord, Generic)

instance Show Exp where
    show = \case
        Clo r n xs   → sexp "{" "}" [show n] xs
        Kal u xs     → sexp "[" "]" [show u] xs
        Rec xs       → sexp "(" ")" ["Rec"] xs
        Ref n xs     → sexp "(" ")" ["V" <> show n] xs
        Reg n xs     → sexp "(" ")" ["R" <> show n] xs
        Iff c t e xs → sexp "(" ")" ["If", show c, show t, show e] xs
        Cas reg x l r xs → sexp "(" ")" ["Case{" <> show reg <> "}", show x, show l, show r] xs
        Let reg x k xs   → sexp "(" ")" ["Let{" <> show reg <> "}", show x, show k] xs
        App x y      → sexp "(" ")" [] [x,y]
      where
        sexp ∷ Show a => String → String → [String] → [a] → String
        sexp _ _ [h] [] = h
        sexp a z hs  xs = a <> intercalate " " (hs <> (show <$> xs)) <> z

prettyExp ∷ Exp → String
prettyExp = go
  where
    go ∷ Exp → String
    go = \case
        Clo r n xs   → sexp "("   ")" [show n] (go <$> xs)
        Kal u xs     → sexp "("   ")" [show u] (go <$> xs)
        Rec xs       → sexp "("   ")" ["$"] (go <$> xs)
        Ref n xs     → sexp "("   ")" [sym (fromIntegral n)] (go <$> xs)
        Reg n xs     → sexp "("   ")" ["R" <> show n] (go <$> xs)
        Iff c t e [] → sexp "?:(" ")" [go c, go t, go e] []
        Iff c t e xs → sexp "("   ")" [go $ Iff c t e []] (go <$> xs)
        Cas _reg x l r [] → sexp "?-(" ")" [go x, go l, go r] []
        Cas reg x l r xs → sexp "("   ")" [go $ Cas reg x l r []] (go <$> xs)
        Let _reg x k []   → sexp "/=(" ")" [go x, go k] []
        Let reg x k xs   → sexp "("   ")" [go $ Let reg x k []] (go <$> xs)
        App x y      → sexp "("   ")" [] [go x, go y]
      where
        sexp ∷ String → String → [String] → [String] → String
        sexp _ _ [h] [] = h
        sexp a z hs  xs = a <> intercalate " " (hs <> xs) <> z

prettyVal = prettyExp . valExp

instance Show Val where
    show = \case
        ValKal u xs     → sexp "[" "]" [show u] xs
        ValRec xs       → sexp "(" ")" ["Rec"] xs
        ValRef n xs     → sexp "(" ")" ["V" <> show n] xs
        ValReg n xs     → sexp "(" ")" ["R" <> show n] xs
        ValIff c t e xs → sexp "(" ")" ["If", show c, show t, show e] xs
        ValCas reg x l r xs → sexp "(" ")" ["Case{" <> show reg <> "}", show x, show l, show r] xs
        ValLet reg x k xs   → sexp "(" ")" ["Let{" <> show reg <> "}", show x, show k] xs
      where
        sexp ∷ Show a => String → String → [String] → [a] → String
        sexp _ _ [h] [] = h
        sexp a z hs  xs = a <> intercalate " " (hs <> (show <$> xs)) <> z

--------------------------------------------------------------------------------

infixl 5 %;

(%) ∷ Exp → Exp → Exp
(%) = App

simplify :: Nat -> Node -> [Exp] -> (Exp, Nat)
simplify nextReg = curry $ \case
  (VS    , [x, y, z] ) -> keep $ (x % z) % (y % z)
  (VK    , [x, y]    ) -> keep x
  (VSeq  , [x, y]    ) -> keep y
  (VIn _ , f : xs    ) -> keep $ app f xs
  (VBn _ , f : g : xs) -> keep $ f % app g xs
  (VCn _ , f : g : xs) -> keep $ app f xs % g
  (VSn _ , f : g : xs) -> keep $ app f xs % app g xs
  (VIff  , [c, t, e] ) -> keep $ Iff c (t % unit) (e % unit) []
  (VCas  , [x, l, r] ) -> incr $ (Cas nextReg x (l % reg nextReg) (r % reg nextReg) [])
  (VLet  , [x, k]    ) -> incr $ (Let nextReg x (k % reg nextReg) [])
  (n     , xs        ) -> error ("simplify: bad arity (" <> show n <> " " <> show xs <> ")")
 where
  keep x = (x, nextReg)
  incr x = (x, succ nextReg)

  app acc = \case
    []     -> acc
    x : xs -> app (acc % x) xs

reg ∷ Nat → Exp
reg n = Reg n []

ref ∷ Nat → Exp
ref n = Ref n []

abst ∷ Exp → Exp
abst = g 0
  where
    g d = \case
        Clo n f xs      → Clo n f (g d <$> xs)
        Kal f xs        → Kal f (g d <$> xs)
        Rec xs          → Rec (g d <$> xs)
        Ref n xs | n>=d → Ref (n+1) (g d <$> xs)
        Ref n xs        → Ref n (g d <$> xs)
        Reg n xs        → Reg n (g d <$> xs) -- TODO
        Iff c t e xs    → Iff (g d c) (g d t) (g d e) (g d <$> xs)
        Cas reg x l r xs    → Cas reg (g d x) (g (d+1) l) (g (d+1) r) (g d <$> xs)
        Let reg x k xs      → Let reg (g d x) (g (d+1) k) (g d <$> xs)
        App x y         → error "TODO: Handle `App` in `abst`"

unit ∷ Exp
unit = Kal F.Uni []

nat ∷ Integral i => i → Nat
nat = fromIntegral

pos ∷ Integral i => i → Pos
pos = fromIntegral

pattern J x = Just x

infixl :@
pattern x :@ y = App x y

nok :: Nat -> Exp -> Maybe (Exp, Nat)
nok nr = go
 where
  go :: Exp -> Maybe (Exp, Nat)
  go = \case
    (go→J (x,r')) :@ y → Just (App x y, r')
    x :@ (go→J (y,r')) → Just (App x y, r')

    --  Because unit is passed to branches, needs further simplification.
    Iff c (go→J (t,r')) e xs → Just (Iff c t e xs, r')
    Iff c t (go→J (e,r')) xs → Just (Iff c t e xs, r')

    --  Result of pattern match is passed into cases on the stack.
    Cas reg v (go→J (l,r')) r xs → Just (Cas reg v l r xs, r')
    Cas reg v l (go→J (r,r')) xs → Just (Cas reg v l r xs, r')
    Let reg v (go→J (k,r')) xs   → Just (Let reg v k xs, r')

    Clo 1 f xs   :@ x → Just $ simplify nr f (snoc xs x)
    Clo n f xs   :@ x → done $ Clo (n-1) f (snoc xs x)
    Kal f xs     :@ x → done $ Kal f (snoc xs x)
    Rec xs       :@ x → done $ Rec (snoc xs x)
    Iff c t e xs :@ x → done $ Iff c t e (snoc xs x)
    Cas reg v l r xs :@ x → done $ Cas reg v l r (snoc xs x)
    Let reg v k xs   :@ x → done $ Let reg v k (snoc xs x)
    Ref n xs     :@ x → done $ Ref n (snoc xs x)
    Reg n xs     :@ x → done $ Reg n (snoc xs x)

    _ → Nothing

  done :: Exp -> Maybe (Exp, Nat)
  done x = Just (x, nr)

{-
    App (Clo 11

    Clo n h exp → pure Nothing
    Kal Ur [Exp]
    Rec [Exp]
    Ref Nat [Exp]
    Iff Exp Exp Exp [Exp]
    Cas Exp Exp Exp [Exp]
    App Exp Exp

    K :@ x :@ y             → Just $ x
    (reduce → Just xv) :@ y → Just $ xv :@ y
    x :@ (reduce → Just yv) → Just $ x :@ yv
    S :@ x :@ y :@ z        → Just $ x :@ z :@ (y :@ z)
    D :@ x                  → Just $ jam x
    J n :@ J 1              → Just $ J (succ n)
    J n :@ t :@ b           → Just $ Fast (fromIntegral n) (match n t b) []
    Fast 0 u us             → Just $ runJet u us
    Fast n u us :@ x        → Just $ Fast (pred n) u (us <> [x])
    _                       → Nothing
-}

call ∷ Nat → Exp → Exp → (Exp, Nat)
call nextReg f x = f & \case
    Clo 1 f xs   → simplify nextReg f (snoc xs x)
    Clo n f xs   → done $ Clo (n-1) f (snoc xs x)
    Kal f xs     → done $ Kal f (snoc xs x)
    Rec xs       → done $ Rec (snoc xs x)
    Ref n xs     → done $ Ref n (snoc xs x)
    Reg n xs     → done $ Reg n (snoc xs x)
    Iff c t e xs → done $ Iff c t e (snoc xs x)
    Cas reg x l r xs → done $ Cas reg x l r (snoc xs x)
    Let reg x k   xs → done $ Let reg x k   (snoc xs x)
    App x y      → error "TODO: Handle `App` in `call` (?)"
 where
  done x = (x, nextReg)

eval :: Nat -> Exp -> IO (Exp, Nat)
eval reg exp = do
  nok reg exp & \case
    Nothing         -> pure (exp, reg)
    Just (e', reg') -> eval reg' e'

nodeRaw :: Nat -> Node -> F.Node
nodeRaw arity = \case
  VS    -> F.Ess
  VK    -> F.Kay
  VIn n -> F.Eye (fromIntegral n)
  VBn n -> F.Bee (fromIntegral n)
  VCn n -> F.Sea (fromIntegral n)
  VSn n -> F.Sen (fromIntegral n)
  VSeq  -> F.Seq
  VIff  -> F.Iff
  VCas  -> F.Cas
  VLet  -> F.Let

evaluate ∷ Exp → IO Val
evaluate = fmap (go . fst) . eval 0
  where
    go = \case
        Clo r n xs   → ValKal (nodeRaw (fromIntegral r) n) (go <$> xs)
        Kal u xs     → ValKal u (go <$> xs)
        Rec xs       → ValRec (go <$> xs)
        Ref n xs     → ValRef n (go <$> xs)
        Reg n xs     → ValReg n (go <$> xs)
        Iff c t e xs → ValIff (go c) (go t) (go e) (go <$> xs)
        Cas reg v l r xs → ValCas reg (go v) (go l) (go r) (go <$> xs)
        Let reg v k xs   → ValLet reg (go v) (go k) (go <$> xs)
        App x y      → trace (show x) $
                       trace (show y) $
                       error "This should not happen"

{-
    If jet has shape `(fix body)`
      Replace with `(body Rec)`
    Then, pass one ref per arity.
    For example: `jetCode 2 "(fix body)"`
      becomes: `(body Rec $1 $0)`
-}
jetCode :: Pos -> F.Val -> F.Val -> IO Code
jetCode arity nm bod = do
  let (ex1, lop) = addRecur bod
  exp <- evaluate $ addArgs (nat arity) ex1
  pure (Code arity nm bod exp lop)
 where

  addArgs :: Nat -> Exp -> Exp
  addArgs 0 x = x
  addArgs n x = addArgs (n - 1) (x % ref (n - 1))

  addRecur :: F.Val -> (Exp, Bool)
  addRecur = \case
    F.VFun (F.Fun 1 F.Fix xs) -> (fastExp (F.getCloN xs 0) % Rec [], True)
    body                      -> (fastExp body, False)

funCode ∷ F.Val → IO Code
funCode body = do
  Code 1 fak fak <$> evaluate (fastExp body % ref 0)
  bExp <- evaluate (fastExp body % ref 0)
  pure (Code 1 fak fak bExp False)
 where
  fak = error "TODO: funCode.fak"

compile ∷ Int → F.Val → F.Val → IO Code
compile n t b = jetCode (fromIntegral n) t b

funVal :: F.Fun -> Val
funVal (F.Fun _ f xs) = ValKal f (fastVal <$> GHC.toList xs)

fastVal :: F.Val -> Val
fastVal = funVal . F.valFun

{-
    x F.:@ y      -> goAcc (go y : acc) x
    F.S           -> ValKal F.Ess acc
    F.K           -> ValKal F.Kay acc
    F.J n         -> ValKal (F.Jay 2) acc
    F.D           -> ValKal F.Dee acc
    F.Fast _ j xs -> ValKal (jRaw j) (fmap go xs <> acc)

  jRaw :: F.Jet -> F.Node
  jRaw = \case
    F.Slow r n b -> F.Jet r (go n) (go b)
    F.Eye        -> F.Eye
    F.Bee        -> F.Bee
    F.Sea        -> F.Sea
    F.Sn n       -> F.Sen n
    F.Bn n       -> F.Ben n
    F.Cn n       -> F.Sea n
    F.JSeq       -> F.Seq
    F.Eye n      -> F.Eye n
    F.JFix       -> F.Fix
    F.JNat n     -> F.Nat n
    F.JBol b     -> F.Bol b
    F.JIff       -> F.Iff
    F.JPak       -> F.Pak
    F.JZer       -> F.Zer
    F.JEql       -> F.Eql
    F.JAdd       -> F.Add
    F.JInc       -> F.Inc
    F.JDec       -> F.Dec
    F.JFec       -> F.Fec
    F.JMul       -> F.Mul
    F.JSub       -> F.Sub
    F.JDed       -> F.Ded
    F.JUni       -> F.Uni
    F.JLef       -> F.Lef
    F.JRit       -> F.Rit
    F.JCas       -> F.Cas
    F.JCon       -> F.Con
    F.JCar       -> F.Car
    F.JCdr       -> F.Cdr
-}

valExp :: Val -> Exp
valExp = go
 where
  go :: Val -> Exp
  go = \case
    ValKal rn xs    -> rawExp rn (go <$> xs)
    ValRec xs       -> Rec (go <$> xs)
    ValRef n xs     -> Ref n (go <$> xs)
    ValReg n xs     -> Reg n (go <$> xs)
    ValIff c t e xs -> Iff (go c) (go t) (go e) (go <$> xs)
    ValCas reg x l r xs -> Cas reg (go x) (go l) (go r) (go <$> xs)
    ValLet reg x k   xs -> Let reg (go x) (go k)        (go <$> xs)

  rawExp :: F.Node -> [Exp] -> Exp
  rawExp rn xs = rn & \case
    F.Kay   -> clo 2 VK
    F.Ess   -> clo 3 VS
    F.Eye n -> clo (int n)     (VIn $ fromIntegral n)
    F.Bee n -> clo (int n + 2) (VBn $ fromIntegral n)
    F.Sea n -> clo (int n + 2) (VCn $ fromIntegral n)
    F.Sen n -> clo (int n + 2) (VSn $ fromIntegral n)
    F.Seq   -> clo 2 VSeq
    F.Iff   -> clo 3 VIff
    F.Cas   -> clo 3 VCas
    F.Let   -> clo 2 VLet
    other  -> kal other
   where
    kal :: F.Node -> Exp
    kal n = Kal n xs

    clo :: Int -> Node -> Exp
    clo r n = Clo (r - args) n xs

    args :: Int
    args = length xs

    int :: Integral i => i -> Int
    int = fromIntegral

fastExp :: F.Val -> Exp
fastExp = valExp . fastVal
