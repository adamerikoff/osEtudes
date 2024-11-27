{-# LANGUAGE ExistentialQuantification #-}

module Main where

import Control.Monad
import Control.Monad.Except
import System.Environment
import Text.ParserCombinators.Parsec hiding (spaces)
import System.IO hiding (try)
import Data.IORef
import Control.Monad.IO.Class (liftIO)  -- Add this import


-- Entry point of the program
main :: IO ()
main = do
    args <- getArgs
    case length args of
        0 -> runRepl
        1 -> evalAndPrint $ args !! 0
        otherwise -> putStrLn "Program takes only 0 or 1 arguments"

-- Parser for symbols
symbol :: Parser Char
symbol = oneOf "!$%&|*+-/:<=?>@^_~#"

-- Function to parse an input string
readExpr :: String -> ThrowsError LispVal
readExpr input = case parse parseExpr "lisp" input of
    Left err  -> throwError $ Parser err
    Right val -> return val  -- Successfully parsed a Lisp expression

-- Parser for spaces
spaces :: Parser ()
spaces = skipMany1 space

-- Data type to represent Lisp values
data LispVal
    = Atom String
    | List [LispVal]
    | DottedList [LispVal] LispVal
    | Number Integer
    | String String
    | Bool Bool

-- Parser for strings
parseString :: Parser LispVal
parseString = do
    _ <- char '"'  -- Start of the string
    x <- many (noneOf "\"")  -- Capture all characters until the closing quote
    _ <- char '"'  -- End of the string
    return $ String x

-- Parser for atoms
parseAtom :: Parser LispVal
parseAtom = do
    first <- letter <|> symbol  -- First character must be a letter or symbol
    rest <- many (letter <|> digit <|> symbol)  -- Remaining characters
    let atom = [first] ++ rest  -- Combine the first and rest into a single string
    return $ case atom of
        "#t" -> Bool True  -- Special case for true
        "#f" -> Bool False  -- Special case for false
        otherwise -> Atom atom  -- All other cases

-- Parser for numbers
parseNumber :: Parser LispVal
parseNumber = liftM (Number . read) $ many1 digit

-- Parser for a list
parseList :: Parser LispVal
parseList = liftM List $ sepBy parseExpr spaces

-- Parser for dotted lists
parseDottedList :: Parser LispVal
parseDottedList = do
    head <- endBy parseExpr spaces
    tail <- char '.' >> spaces >> parseExpr
    return $ DottedList head tail

-- Parser for quoted expressions
parseQuoted :: Parser LispVal
parseQuoted = do
    _ <- char '\''
    x <- parseExpr
    return $ List [Atom "quote", x]

-- Parser for Lisp expressions
parseExpr :: Parser LispVal
parseExpr = parseAtom
    <|> parseString
    <|> parseNumber
    <|> parseQuoted
    <|> do
        _ <- char '('
        x <- (try parseList) <|> parseDottedList
        _ <- char ')'
        return x

showVal :: LispVal -> String
showVal (String contents) =  "\"" ++ contents ++ "\""
showVal (Atom name) = name
showVal (Number contents) = show contents
showVal (Bool True) = "#t"
showVal (Bool False) = "#f"
showVal (List contents) = "(" ++ unwordsList contents ++ ")"
showVal (DottedList head tail) = "(" ++ unwordsList head ++ " . " ++ showVal tail ++ ")"

unwordsList :: [LispVal] -> String
unwordsList = unwords . map showVal

instance Show LispVal where show = showVal

-- Evaluate a Lisp expression in a given environment
eval :: Env -> LispVal -> IOThrowsError LispVal
-- Return values for basic types
eval env val@(String _) = return val
eval env val@(Number _) = return val
eval env val@(Bool _) = return val

-- Look up a variable in the environment
eval env (Atom id) = getVar env id

-- Handle the quote special form
eval env (List [Atom "quote", val]) = return val

-- Handle the if special form
eval env (List [Atom "if", pred, conseq, alt]) = do
    result <- eval env pred
    case result of
        Bool False -> eval env alt
        Bool True  -> eval env conseq
        _          -> throwError $ TypeMismatch "boolean" result

-- Handle the set! special form
eval env (List [Atom "set!", Atom var, form]) =
    eval env form >>= setVar env var

-- Handle the define special form
eval env (List [Atom "define", Atom var, form]) =
    eval env form >>= defineVar env var

-- Function application
eval env (List (Atom func : args)) = do
    argVals <- mapM (eval env) args
    liftThrows $ apply func argVals

-- Catch unrecognized forms
eval env badForm =
    throwError $ BadSpecialForm "Unrecognized special form" badForm

-- Apply a function to arguments
apply :: String -> [LispVal] -> ThrowsError LispVal
apply func args =
    maybe (throwError $ NotFunction "Unrecognized primitive function" func)
          ($ args) -- If found, apply the function to the arguments
          (lookup func primitives)


primitives :: [(String, [LispVal] -> ThrowsError LispVal)]
primitives = [
    ("+", numericBinop (+)),
    ("-", numericBinop (-)),
    ("*", numericBinop (*)),
    ("/", numericBinop div),
    ("mod", numericBinop mod),
    ("quotient", numericBinop quot),
    ("remainder", numericBinop rem),
    ("=", numBoolBinop (==)),
    ("<", numBoolBinop (<)),
    (">", numBoolBinop (>)),
    ("/=", numBoolBinop (/=)),
    (">=", numBoolBinop (>=)),
    ("<=", numBoolBinop (<=)),
    ("&&", boolBoolBinop (&&)),
    ("||", boolBoolBinop (||)),
    ("string=?", strBoolBinop (==)),
    ("string?", strBoolBinop (>)),
    ("string<=?", strBoolBinop (<=)),
    ("string>=?", strBoolBinop (>=)),
    ("car", car),
    ("cdr", cdr),
    ("cons", cons),
    ("eq?", eqv),
    ("eqv?", eqv),
    ("equal?", equal)]

numericBinop :: (Integer -> Integer -> Integer) -> [LispVal] -> ThrowsError LispVal
numericBinop op [singleVal] = throwError $ NumArgs 2 singleVal
numericBinop op params = mapM unpackNum params >>= return . Number . foldl1 op

-- Boolean operations with unpackers
boolBinop :: (LispVal -> ThrowsError a) -> (a -> a -> Bool) -> [LispVal] -> ThrowsError LispVal
boolBinop unpacker op args =
    if length args /= 2
        then throwError $ NumArgs 2 args
        else do
            left <- unpacker $ args !! 0
            right <- unpacker $ args !! 1
            return $ Bool $ left `op` right

numBoolBinop = boolBinop unpackNum
strBoolBinop = boolBinop unpackStr
boolBoolBinop = boolBinop unpackBool

-- Unpacking numbers
unpackNum :: LispVal -> ThrowsError Integer
unpackNum (Number n) = return n
unpackNum (String n) =
    let parsed = reads n
    in if null parsed
        then throwError $ TypeMismatch "number" $ String n
        else return $ fst $ parsed !! 0
unpackNum (List [n]) = unpackNum n
unpackNum notNum = throwError $ TypeMismatch "number" notNum

-- Unpacking strings
unpackStr :: LispVal -> ThrowsError String
unpackStr (String s) = return s
unpackStr (Number s) = return $ show s
unpackStr (Bool s) = return $ show s
unpackStr notString = throwError $ TypeMismatch "string" notString

-- Unpacking booleans
unpackBool :: LispVal -> ThrowsError Bool
unpackBool (Bool b) = return b
unpackBool notBool = throwError $ TypeMismatch "boolean" notBool

-- car implementation
car :: [LispVal] -> ThrowsError LispVal
car [List (x:xs)] = return x
car [DottedList (x:xs) _] = return x
car [badArg] = throwError $ TypeMismatch "pair" badArg
car badArgList = throwError $ NumArgs 1 badArgList

-- cdr implementation
cdr :: [LispVal] -> ThrowsError LispVal
cdr [List (_:xs)] = return $ List xs
cdr [DottedList [_] x] = return x
cdr [DottedList (_:xs) x] = return $ DottedList xs x
cdr [badArg] = throwError $ TypeMismatch "pair" badArg
cdr badArgList = throwError $ NumArgs 1 badArgList

-- cons implementation
cons :: [LispVal] -> ThrowsError LispVal
cons [x1, List []] = return $ List [x1]
cons [x, List xs] = return $ List $ x : xs
cons [x, DottedList xs xlast] = return $ DottedList (x : xs) xlast
cons [x1, x2] = return $ DottedList [x1] x2
cons badArgList = throwError $ NumArgs 2 badArgList

-- eqv implementation
eqv :: [LispVal] -> ThrowsError LispVal
eqv [(Bool arg1), (Bool arg2)] = return $ Bool $ arg1 == arg2
eqv [(Number arg1), (Number arg2)] = return $ Bool $ arg1 == arg2
eqv [(String arg1), (String arg2)] = return $ Bool $ arg1 == arg2
eqv [(Atom arg1), (Atom arg2)] = return $ Bool $ arg1 == arg2
eqv [(DottedList xs x), (DottedList ys y)] = eqv [List (xs ++ [x]), List (ys ++ [y])]
eqv [(List arg1), (List arg2)] =
    return $ Bool $ (length arg1 == length arg2) &&
        (all eqvPair $ zip arg1 arg2)
    where eqvPair (x1, x2) = case eqv [x1, x2] of
                                Left _ -> False
                                Right (Bool val) -> val
eqv [_, _] = return $ Bool False
eqv badArgList = throwError $ NumArgs 2 badArgList

-- Unpacker type for general equality checks
data Unpacker = forall a. Eq a => AnyUnpacker (LispVal -> ThrowsError a)

unpackEquals :: LispVal -> LispVal -> Unpacker -> ThrowsError Bool
unpackEquals arg1 arg2 (AnyUnpacker unpacker) =
    (do unpacked1 <- unpacker arg1
        unpacked2 <- unpacker arg2
        return $ unpacked1 == unpacked2)
    `catchError` (const $ return False)

-- General equality function
equal :: [LispVal] -> ThrowsError LispVal
equal [arg1, arg2] = do
    primitiveEquals <- liftM or $ mapM (unpackEquals arg1 arg2)
        [AnyUnpacker unpackNum, AnyUnpacker unpackStr, AnyUnpacker unpackBool]
    eqvEquals <- eqv [arg1, arg2]
    return $ Bool $ primitiveEquals || let (Bool x) = eqvEquals in x
equal badArgList = throwError $ NumArgs 2 badArgList

-- Error data type and show instance
data LispError = NumArgs Integer [LispVal]
               | TypeMismatch String LispVal
               | Parser ParseError
               | BadSpecialForm String LispVal
               | NotFunction String String
               | UnboundVar String String
               | Default String

showError :: LispError -> String
showError (UnboundVar message varname) = message ++ ": " ++ varname
showError (BadSpecialForm message form) = message ++ ": " ++ show form
showError (NotFunction message func) = message ++ ": " ++ show func
showError (NumArgs expected found) = "Expected " ++ show expected ++ " args; found values " ++ unwordsList found
showError (TypeMismatch expected found) = "Invalid type: expected " ++ expected ++ ", found " ++ show found
showError (Parser parseErr) = "Parse error at " ++ show parseErr

instance Show LispError where show = showError

type ThrowsError = Either LispError

trapError action = catchError action (return . show)

-- Extract value from ThrowsError (Right value)
extractValue :: ThrowsError a -> a
extractValue (Right val) = val
extractValue (Left err) = error $ "Error: " ++ show err

-- Flush string to stdout
flushStr :: String -> IO ()
flushStr str = putStr str >> hFlush stdout

-- Read user prompt
readPrompt :: String -> IO String
readPrompt prompt = flushStr prompt >> getLine

-- Evaluate and print expression
evalAndPrint :: Env -> String -> IO ()
evalAndPrint env expr = evalString env expr >>= putStrLn

-- Evaluate expression from string
evalString :: Env -> String -> IO String
evalString env expr = runIOThrows $ liftM show (liftThrows $ readExpr expr) >>= eval env

-- Repeatedly perform action while condition holds
until_ :: Monad m => (a -> Bool) -> m a -> (a -> m ()) -> m ()
until_ pred prompt action = do
  result <- prompt
  if pred result
    then return ()
    else action result >> until_ pred prompt action

-- Run one expression and print result
runOne :: String -> IO ()
runOne expr = nullEnv >>= flip evalAndPrint expr

-- Run REPL (Read-Eval-Print Loop)
runRepl :: IO ()
runRepl = nullEnv >>= evalAndPrintUntilQuit

-- Run REPL until user inputs "quit"
evalAndPrintUntilQuit :: Env -> IO ()
evalAndPrintUntilQuit env = until_ (=="quit") (readPrompt "Lisp >>> ") (evalAndPrint env)

-- Define the environment
type Env = IORef [(String, IORef LispVal)]

-- Create an empty environment
nullEnv :: IO Env
nullEnv = newIORef []

-- IOThrowsError type for handling errors
type IOThrowsError = ExceptT LispError IO

-- Lift a ThrowsError into IOThrowsError
liftThrows :: ThrowsError a -> IOThrowsError a
liftThrows (Left err) = throwError err
liftThrows (Right val) = return val

-- Run an IOThrowsError action
runIOThrows :: IOThrowsError String -> IO String
runIOThrows action = runExceptT (trapError extractValue action) >>= return

-- Check if a variable is bound in the environment
isBound :: Env -> String -> IO Bool
isBound envRef var = readIORef envRef >>= return . maybe False (const True) . lookup var

-- Get the value of a variable from the environment
getVar :: Env -> String -> IOThrowsError LispVal
getVar envRef var = do
  env <- liftIO $ readIORef envRef
  maybe (throwError $ UnboundVar "Getting unbound variable" var)
        (liftIO . readIORef)
        (lookup var env)

-- Set the value of a variable in the environment
setVar :: Env -> String -> LispVal -> IOThrowsError LispVal
setVar envRef var value = do
  env <- liftIO $ readIORef envRef
  maybe (throwError $ UnboundVar "Setting unbound variable" var)
        (liftIO . flip writeIORef value)
        (lookup var env)
  return value

-- Define a variable in the environment
defineVar :: Env -> String -> LispVal -> IOThrowsError LispVal
defineVar envRef var value = do
  alreadyDefined <- isBound envRef var
  if alreadyDefined
    then setVar envRef var value >> return value
    else liftIO $ do
      valueRef <- newIORef value
      env <- readIORef envRef
      writeIORef envRef ((var, valueRef) : env)
      return value

-- Bind variables in the environment
bindVars :: Env -> [(String, LispVal)] -> IO Env
bindVars envRef bindings = do
  env <- readIORef envRef
  extendedEnv <- liftM (++ env) (mapM addBinding bindings)
  writeIORef envRef extendedEnv
  return envRef
  where
    addBinding (var, value) = do
      ref <- newIORef value
      return (var, ref)