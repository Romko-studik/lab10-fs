# Lab work 10: fs

Authors (team):


-   [Roman Zaletskyi](https://github.com/Romko-studik)

### Compilation

```shell
mkdir build
cd build
cmake ..
make
```

### Usage

```shell
./myshell file.sh
```

Or for interactive mode:

```shell
./myshell
```

Alternatively, use a shebang in the script file:

```shell
#!/path/to/myshell

# Your code here
```

This allows you to launch `./script.msh` from within the shell as well.

# Additional tasks

-   Локальні змінні середовища (export + shell variables):
    You can set environment variables using the `export` command. For example:

```shell
export MY_VAR=123
echo $MY_VAR
```

You can first set a variable and then export it:

```shell
MY_VAR=123
export MY_VAR
echo $MY_VAR
```

If a variable is already in the environment, you can change its value without exporting it again:

```shell
export MY_VAR=123
MY_VAR=456
echo $MY_VAR
```

You can also set the variable for just a single command:

```shell
MY_VAR=123 ./my_var_echoer.sh
```

An example of a shell variable created by default (if not already in the environment) is `HISTFILE`.
