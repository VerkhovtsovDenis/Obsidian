### Отображение кириллицы в консоли git bash

Для отключения преобразования русских символов в строки вида \320\236\321\202\320  только в текущем хранилище выполните:

```bash
$ git config core.quotepath false
```

для глобального отключения добавьте опцию `--global`:

```bash
$ git config --global core.quotepath false
```

[Материал на StackOverFlow](https://ru.stackoverflow.com/questions/770949/%D0%A0%D1%83%D1%81%D0%B8%D1%84%D0%B8%D0%BA%D0%B0%D1%86%D0%B8%D1%8F-git-%D0%B2-%D0%BA%D0%BE%D0%BD%D1%81%D0%BE%D0%BB%D0%B8)

### Выполнение add-commit одной командой

Для выполнения команд git add . и git commit  в одной строчке можно использовать псевдоним add-commit:

```bash
git config --global alias.add-commit '!git add -A && git commit'
```

Пример использования

```bash
git add-commit -m 'My commit message'
```

[Материал на StackOverFlow](https://stackoverflow.com/questions/4298960/git-add-and-commit-in-one-command)