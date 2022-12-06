{-# LANGUAGE ForeignFunctionInterface #-}

module Day_5 where

import Foreign.C.Types
import Foreign.C (CString, peekCString)

import Data.List
import Data.List.Split
import Data.Maybe (fromJust, fromMaybe)

import Text.Read (readMaybe)

-- Stack type
newtype Stack a = Stack [a] deriving (Eq, Ord, Show)

empty :: Stack a
empty = Stack []

push :: a -> Stack a -> Stack a
push x (Stack s) = Stack (x:s)

pop :: Stack a -> (Maybe a, Stack a)
pop (Stack [])     = (Nothing, Stack [])
pop (Stack (x:xs)) = (Just x, Stack xs)

pushGroup :: Stack a -> Stack a -> Stack a
pushGroup (Stack top) (Stack bottom) = Stack (top ++ bottom)

popGroup :: Int -> Stack a -> (Stack a, Stack a)
popGroup num (Stack s)
    | num <= length s = (Stack (fst arrays), Stack (snd arrays))
    | otherwise       = (empty, Stack s)
    where
        arrays = splitAt num s

-- util
replaceAt :: Int -> a -> [a] -> [a]
replaceAt index elem l = pre ++ elem : post
    where (pre, _: post) = splitAt index l

readLines :: FilePath -> IO [String]
readLines = fmap lines . readFile

-- stack parser
addStackLine :: [Stack Char] -> [(Int, Char)] -> ([Stack Char], String)
addStackLine stacks [] = (stacks, "end")
addStackLine stacks ((index, ch):rest)
    | ch == ' '             = (fst $ addStackLine stacks rest, "skip")
    | index < length stacks = (fst $ addStackLine modStacks rest, "push" ++ [ch] ++ " " ++ show index)
    | otherwise             = (fst $ addStackLine withNew rest, "make" ++ [ch] ++ " " ++ show index)
    where
        curr      = stacks !! index
        pushed    = push ch curr
        modStacks = replaceAt index pushed stacks --pre ++ pushed : post
        newStack  = Stack [ch]
        withNew   = stacks ++ [newStack]

buildStacks :: [String] -> [Stack Char]
buildStacks cubes = fst accum
    where accum = mapAccumR addStackLine [] (map (zip [0..]) cubes)

-- move parser
type MoveInstruction = (Int, Int, Int)

-- no idea how to handle maybes gracefully
parseMoveInstruction :: String -> MoveInstruction
parseMoveInstruction str
    | length chunks /= 6 = (0, 0, 0)
    | otherwise          = (num, from - 1, to - 1)
    where
        chunks = splitOn " " str
        num    = fromMaybe 0 (readMaybe (chunks !! 1))
        from   = fromMaybe 0 (readMaybe (chunks !! 3))
        to     = fromMaybe 0 (readMaybe (chunks !! 5))

moveIndividual :: [Stack Char] -> MoveInstruction -> ([Stack Char], String)
moveIndividual stacks (0, _, _) = (stacks, "end")
moveIndividual stacks (num, from, to) =
    (fst $ moveIndividual newStacks (num - 1, from, to), "moving " ++ show popped ++ " to " ++ show pushed)
    where
        popped = pop (stacks !! from)
        pushed =
            case fst popped of
                Nothing -> stacks !! to
                Just ch -> push ch (stacks !! to)
        newStacks = replaceAt to pushed $ replaceAt from (snd popped) stacks

moveStacks :: [Stack Char] -> MoveInstruction -> ([Stack Char], String)
moveStacks stacks (num, from, to) =
    (newStacks, "moving " ++ show popped ++ " to " ++ show pushed)
    where
        popped = popGroup num (stacks !! from)
        pushed = pushGroup (fst popped) (stacks !! to)
        newStacks = replaceAt to pushed $ replaceAt from (snd popped) stacks

movePart1 :: [Stack Char] -> [MoveInstruction] -> ([Stack Char], [String])
movePart1 = mapAccumL moveIndividual

movePart2 :: [Stack Char] -> [MoveInstruction] -> ([Stack Char], [String])
movePart2 = mapAccumL moveStacks

day5 :: String -> Int -> IO Bool
day5 filename part = do
    lines <- readLines filename
    let (stackLines, moveLines) = splitAt (fromJust (elemIndex "" lines)) lines
    let cubes = map (map (!! 1) . chunksOf 4) (init stackLines)

    let stacks = buildStacks cubes
    let moves = map parseMoveInstruction (tail moveLines)

    let moveFn = if part == 0 then movePart1 else movePart2

    let moved = moveFn stacks moves

    let tops = map (fromMaybe ' ' . fst . pop) $ fst moved
    print tops

    return True

-- day_5_hs :: CString -> IO Bool
-- day_5_hs cstr = day5 (peekCString cstr)

-- foreign export ccall day_5_hs :: CString -> IO Bool
