{-|
    Code for setting up the RIO environment.
-}
module Urbit.King.App
  ( KingEnv
  , runKingEnvStderr
  , runKingEnvLogFile
  , runKingEnvNoLog
  , PierEnv
  , runPierEnv
  , HasStderrLogFunc(..)
  , HasKingId(..)
  , HasProcId(..)
  , HasKingEnv(..)
  , HasPierEnv(..)
  , module Urbit.King.Config
  )
where

import Urbit.King.Config
import Urbit.Prelude

import System.Directory       (createDirectoryIfMissing, getHomeDirectory)
import System.Posix.Internals (c_getpid)
import System.Posix.Types     (CPid(..))
import System.Random          (randomIO)


-- KingEnv ---------------------------------------------------------------------

class HasStderrLogFunc a where
  stderrLogFuncL :: Lens' a LogFunc

class HasKingId a where
  kingIdL :: Lens' a Word16

class HasProcId a where
  procIdL :: Lens' a Int32

class (HasLogFunc a, HasStderrLogFunc a, HasKingId a, HasProcId a)
   => HasKingEnv a
 where
  kingEnvL :: Lens' a KingEnv

data KingEnv = KingEnv
  { _kingEnvLogFunc       :: !LogFunc
  , _kingEnvStderrLogFunc :: !LogFunc
  , _kingEnvKingId        :: !Word16
  , _kingEnvProcId        :: !Int32
  }

makeLenses ''KingEnv

instance HasKingEnv KingEnv where
  kingEnvL = id

instance HasLogFunc KingEnv where
  logFuncL = kingEnvLogFunc

instance HasStderrLogFunc KingEnv where
  stderrLogFuncL = kingEnvStderrLogFunc

instance HasProcId KingEnv where
  procIdL = kingEnvProcId

instance HasKingId KingEnv where
  kingIdL = kingEnvKingId


-- Running KingEnvs ------------------------------------------------------------

runKingEnvStderr :: RIO KingEnv a -> IO a
runKingEnvStderr inner = do
  logOptions <-
    logOptionsHandle stderr True <&> setLogUseTime True <&> setLogUseLoc False

  withLogFunc logOptions $ \logFunc -> runKingEnv logFunc logFunc inner

runKingEnvLogFile :: RIO KingEnv a -> IO a
runKingEnvLogFile inner = withLogFileHandle $ \h -> do
  logOptions <-
    logOptionsHandle h True <&> setLogUseTime True <&> setLogUseLoc False
  stderrLogOptions <-
    logOptionsHandle stderr True <&> setLogUseTime False <&> setLogUseLoc False

  withLogFunc stderrLogOptions $ \stderrLogFunc -> withLogFunc logOptions
    $ \logFunc -> runKingEnv logFunc stderrLogFunc inner

withLogFileHandle :: (Handle -> IO a) -> IO a
withLogFileHandle act = do
  home <- getHomeDirectory
  let logDir = home </> ".urbit"
  createDirectoryIfMissing True logDir
  withFile (logDir </> "king.log") AppendMode $ \handle -> do
    hSetBuffering handle LineBuffering
    act handle

runKingEnvNoLog :: RIO KingEnv a -> IO a
runKingEnvNoLog act = withFile "/dev/null" AppendMode $ \handle -> do
  logOptions <- logOptionsHandle handle True
  withLogFunc logOptions $ \logFunc -> runKingEnv logFunc logFunc act

runKingEnv :: LogFunc -> LogFunc -> RIO KingEnv a -> IO a
runKingEnv logFunc stderr action = do
  kid      <- randomIO
  CPid pid <- c_getpid
  runRIO (KingEnv logFunc stderr kid pid) action


-- PierEnv ---------------------------------------------------------------------

class (HasKingEnv a, HasPierConfig a, HasNetworkConfig a) => HasPierEnv a where
  pierEnvL :: Lens' a PierEnv

data PierEnv = PierEnv
  { _pierEnvKingEnv       :: !KingEnv
  , _pierEnvPierConfig    :: !PierConfig
  , _pierEnvNetworkConfig :: !NetworkConfig
  }

makeLenses ''PierEnv

instance HasKingEnv PierEnv where
  kingEnvL = pierEnvKingEnv

instance HasPierEnv PierEnv where
  pierEnvL = id

instance HasKingId PierEnv where
  kingIdL = kingEnvL . kingEnvKingId

instance HasStderrLogFunc PierEnv where
  stderrLogFuncL = kingEnvL . stderrLogFuncL

instance HasLogFunc PierEnv where
  logFuncL = kingEnvL . logFuncL

instance HasPierPath PierEnv where
  pierPathL = pierEnvPierConfig . pierPathL

instance HasDryRun PierEnv where
  dryRunL = pierEnvPierConfig . dryRunL

instance HasPierConfig PierEnv where
  pierConfigL = pierEnvPierConfig

instance HasNetworkConfig PierEnv where
  networkConfigL = pierEnvNetworkConfig

instance HasProcId PierEnv where
  procIdL = kingEnvL . kingEnvProcId


-- Running Pier Envs -----------------------------------------------------------

runPierEnv :: PierConfig -> NetworkConfig -> RIO PierEnv a -> RIO KingEnv a
runPierEnv pierConfig networkConfig action = do
  app <- ask

  let pierEnv = PierEnv { _pierEnvKingEnv       = app
                        , _pierEnvPierConfig    = pierConfig
                        , _pierEnvNetworkConfig = networkConfig
                        }

  io (runRIO pierEnv action)
