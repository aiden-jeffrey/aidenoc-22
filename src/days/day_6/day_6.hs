{-# LANGUAGE ForeignFunctionInterface #-}

module Day_6 where

import Foreign.C.Types
import Foreign.C (CString, peekCString)

import Data.List
import Data.Maybe (fromMaybe)

-- util
replaceAt :: Int -> a -> [a] -> [a]
replaceAt index elem l = pre ++ elem : post
    where (pre, _: post) = splitAt index l

readLines :: FilePath -> IO [String]
readLines = fmap lines . readFile

-- parser
lookForSignal :: Int -> String -> Char -> (String, Char)
lookForSignal cnt prev ch = (next, if isStart then '!' else '-')
    where
        curr    = prev ++ [ch]
        full    = length curr >= cnt
        next    = if full then tail curr else curr
        isStart = full && (length (nub curr) == length curr)

processSignal :: Int -> String -> Int
processSignal cnt = (+1) . fromMaybe (-2) . index
    where index = elemIndex '!' . snd . mapAccumL (lookForSignal cnt) ""

day6 :: String -> Int -> IO Bool
day6 filename part = do
    lines <- readLines filename

    let cnt = if part == 0 then 4 else 14

    let positions = map (processSignal cnt) lines

    print positions

    return True

-- day_5_hs :: CString -> IO Bool
-- day_5_hs cstr = day5 (peekCString cstr)

-- foreign export ccall day_5_hs :: CString -> IO Bool
