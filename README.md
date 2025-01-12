# Lisp Compiler

這是一個用 C++ 實作的 Lisp 編譯器，旨在將 Common Lisp 程式碼轉成 C++ ，並編譯為 AT&T 語法的 x86-64 組合語言。

## 專案架構

- Tokenizer : 用了 [PEGTL](https://github.com/taocpp/PEGTL) 套件，因為 C++ 原生的 Regular Expression 套件效能太慢了。
- Parser : 只有將最原始的的結構先生成出來，因為要處理 `'` ，實作上可以省去很多麻煩。
- Generator : 實際上他整合了一部分 type checking 及語法分析，主要的功能是把 AST 生成 C++ 代碼。
- Type Checking : 因為有些函數定義在生成的 C++ 代碼中，因此 Type Checking 的實作分在 Generator 與生成的 C++ 中。

生成出的 C++ 會有很大一部份是預先編寫的內容，因為有一部份的函數定義在裡面。

## 專案結構

```
.
├── build.sh
├── .clang-format
├── CMakeLists.txt
├── .gitignore
├── README.md
├── src
│   ├── ast.cpp
│   ├── ast.h
│   ├── generator.cpp
│   ├── generator.h
│   ├── main.cpp
│   ├── parser.cpp
│   ├── parser.h
│   ├── rule.h
│   ├── template.h
│   └── token.h
├── test
│   ├── compile
│   │   ├── test_boolean.lisp
│   │   ├── test_control.lsp
│   │   ├── test_defun.lisp
│   │   ├── test_let.lisp
│   │   ├── test_list.lisp
│   │   ├── test_operators.lisp
│   │   └── test_print.lisp
│   ├── compile-fail
│   │   ├── invalid_let.lisp
│   │   ├── missing_closing_parenthesis.lisp
│   │   ├── unbounded_1.lisp
│   │   ├── unbounded_2.lisp
│   │   ├── unbounded_3.lisp
│   │   └── unclosed_comment.lisp
│   ├── exec
│   │   ├── factorial.lisp
│   │   ├── list_length.lisp
│   │   ├── middle.cpp
│   │   ├── power.lisp
│   │   └── square.lisp
│   └── exec-fail
│       ├── division_by_zero.lisp
│       ├── undefined_car.lisp
│       ├── undefined_cdr.lisp
│       └── undefined_equal.lisp
└── test.sh
```

## 構建專案

執行 build.sh 就可以建構出 compiler 的執行檔，名稱為 lisp-compiler。(需要 C++ 20 及 CMake，理論上 C++ 17 應該就可以了，但我還沒試過)

## 測試專案

執行完 build.sh 後執行 test.sh 就可以測試所有測試。
這個測試會需要在系統上安裝 SBCL，並將其產生的輸出與專案的做比對，判斷輸出正確與否。

## 使用編譯器

編譯器的使用方法如下：
```bash
./lisp-compiler <source.lisp>
```
這將會將 `source.lisp` 檔案編譯為 at&t syntax 的 x86-64 組合語言及執行檔。

## 測試結果

```
[PASS] Successfully compiled: test/compile/test_boolean.lisp
[PASS] Successfully compiled: test/compile/test_defun.lisp
[PASS] Successfully compiled: test/compile/test_let.lisp
[PASS] Successfully compiled: test/compile/test_list.lisp
[PASS] Successfully compiled: test/compile/test_operators.lisp
[PASS] Successfully compiled: test/compile/test_print.lisp
[PASS] Successfully compiled: test/compile/test_control.lsp
[PASS] Correctly failed to compile: test/compile-fail/invalid_let.lisp
[PASS] Correctly failed to compile: test/compile-fail/missing_closing_parenthesis.lisp
[PASS] Correctly failed to compile: test/compile-fail/unbounded_1.lisp
[PASS] Correctly failed to compile: test/compile-fail/unbounded_2.lisp
[PASS] Correctly failed to compile: test/compile-fail/unbounded_3.lisp
[PASS] Correctly failed to compile: test/compile-fail/unclosed_comment.lisp
[PASS] Output matches: test/exec/factorial.lisp
[PASS] Output matches: test/exec/list_length.lisp
[PASS] Output matches: test/exec/power.lisp
[PASS] Output matches: test/exec/square.lisp
[PASS] Correctly failed at runtime: test/exec-fail/division_by_zero.lisp
[PASS] Correctly failed at runtime: test/exec-fail/undefined_car.lisp
[PASS] Correctly failed at runtime: test/exec-fail/undefined_cdr.lisp
[PASS] Correctly failed at runtime: test/exec-fail/undefined_equal.lisp
Total tests: 21
Passed tests: 21
Success rate: 100%
```
