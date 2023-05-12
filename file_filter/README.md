# file_filter

搜索并移动文件的命令行程序。

用法

```sh
file_filter [-l <lineNo>] -s <searchingString> <destinationPath>
```

递归搜索当前目录，将第 `<lineNo>` 行包含 `<searchingString>` 的 `.txt` 文件移动到 `<destinationPath>`.
