{-

This parser example has adjustable maximum depth levels for
parenthesized expressions and for raw text runs. Once those levels are
reached, it ceases to store children of parenthesized expressions
(stores their raw contents only), and concatenates the tokens of raw
text runs.

-}

{-# LANGUAGE OverloadedStrings #-}
import Data.Attoparsec.ByteString.Char8
import Data.Tree
import qualified Data.ByteString.Char8 as BS
import Control.Applicative (many, some, (<|>))
import System.Environment
import Numeric

pTree :: (Int, Int, Int) -> Parser (Tree BS.ByteString)
pTree (level, maxDepthP, maxDepthW) =
  (uncurry Node . (\(r, c) ->
                     (r, if level < maxDepthP then c else []))
    <$> (char '(' *>
         match (pForest (level + 1, maxDepthP, maxDepthW))
         <* char ')'))
  <|> flip Node [] . BS.pack <$>
  (if level < maxDepthW then id else (\p -> concat <$> some p))
  (choice
   [ some (satisfy (inClass " \n"))
   , some (satisfy (notInClass " \n()\\")
           <|> (char '\\' *> satisfy (inClass " \n()\\")))])

pForest :: (Int, Int, Int) -> Parser (Forest BS.ByteString)
pForest = many . pTree

escape :: BS.ByteString -> BS.ByteString
escape s
  | BS.all (inClass " \n") s = s
  | otherwise = BS.concatMap
    (\c -> BS.pack $ if inClass " \n()\\" c then  ['\\', c] else [c]) s

unescape' :: [Char] -> [Char]
unescape' [] = []
unescape' ('\\':c:rest) = c : unescape' rest
unescape' (c:rest) = c : unescape' rest

unescape :: BS.ByteString -> BS.ByteString
unescape = BS.pack . unescape' . BS.unpack

showTree :: Tree BS.ByteString -> BS.ByteString
showTree (Node val [])
  | BS.null val = "()"
  | otherwise = escape val
showTree (Node _ children) =
  BS.concat ["(", showForest children, ")"]

showForest :: Forest BS.ByteString -> BS.ByteString
showForest = BS.concat . map showTree

main :: IO ()
main = do
  as <- getArgs
  let (maxDepthP, maxDepthW) = case as of
        [p', w'] -> case (readDec p', readDec w') of
          ([(p, "")], [(w, "")]) -> (p, w)
          _ -> (100, 100)
        _ -> (100, 100)
  v <- parseOnly (pForest (0, maxDepthP, maxDepthW) <* endOfInput)
    <$> BS.getContents
  case v of
    Right v' -> do
      print v'
      BS.putStrLn $ showForest v'
    Left e -> putStrLn $ "Parse error: " ++ e
