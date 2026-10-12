#import "@preview/cetz:0.5.2"

#set page(
  paper: "a4",
  margin: (x: 1cm, y: 1cm),
)

#show math.equation.where(block: true): set align(left)

Я не математик, обновляю материалы по мере собственного обучения, поэтому могу допускать ошибки и неточности, осторожнее с этим!

= 1. Доказать, что $|a| = | -a |$, $a in bb(R)$ <proof-1>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-1">see extra</a>

Определение модуля:

$ |a| = cases(
  a &", если" a >= 0,
  -a &", если" a < 0,
) $

Если $a>0$, то $|a|=a$ и $|-a|=a => |a|=|-a|$\
Если $a<0$, то $|a|=-a$ и $|-a|=-a => |a|=|-a|$\
Если $a=0$, то $|a|=0$ и $|-a|=0 => |a|=|-a|$\

= 2. Доказать, что $|a-b| = |b-a|, a "и" b in bb(R)$ <proof-2>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-2">see extra</a>

Пусть $c=a-b$, тогда $|c|=|-c| => |a-b|=|b-a|$

= 3. Доказать, что $|a^2|=|-a^2|=a^2$ <proof-3>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-3">see extra</a>

Пусть $c=a^2$, тогда $|c| = |a^2| = a^2$ (по определению модуля)

$|c| = |-c| => |a^2| =|-a^2| = a^2$

= 4. Доказать, что сумма квадратов двух чисел не меньше удвоенного произведения этих чисел <proof-4>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-4">see extra</a>

$(a-b)^2 = a^2 -2 a b + b^2 >= 0 => a^2 + b^2 >= 2 a b$

= 5. Доказать свойство модуля: если для чисел $x$ и $C>=0$ выполняется двойное неравенство $-C <= x <= C$, то $|x| <= C$ <proof-5>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-5">see extra</a>

Ситуация 1: $-C <= x <= 0 => |x| <= C (-С <= x <= 0)$ (по определению модуля)

#cetz.canvas({
  import cetz.draw: *
  
  // стиль штриховки
  let stroke-style = (paint: gray.lighten(20%), thickness: 0.5pt)
  
  let start-x = -2.0
  let end-x = 0.0
  let step = 0.15
  let h = 0.3
  
  // штриховка
  let x = start-x
  while x <= end-x {
    line((x, -h), (x + 0.15, h), stroke: stroke-style)
    x = x + step
  }
  
  // числовая ось
  line((-3.5, 0), (3.5, 0), mark: (end: ">"), stroke: 1pt)
  
  // точка -C
  line((-2, -0.1), (-2, 0.1), stroke: 1pt)
  content((-2, -0.4), [$-C$])
  
  // точка 0
  line((0, -0.1), (0, 0.1), stroke: 1pt)
  content((0, -0.4), [$0$])
  
  // точка C
  line((2, -0.1), (2, 0.1), stroke: 1pt)
  content((2, -0.4), [$C$])
})

Ситуация 2: $0 <= x <= C => |x| <= C (0 <= x <= C)$ (снова по определению модуля)

#cetz.canvas({
  import cetz.draw: *
  
  // стиль штриховки
  let stroke-style = (paint: gray.lighten(20%), thickness: 0.5pt)
  
  let start-x = 0
  let end-x = 2
  let step = 0.15
  let h = 0.3
  
  // штриховка
  let x = start-x
  while x <= end-x {
    line((x, -h), (x + 0.15, h), stroke: stroke-style)
    x = x + step
  }
  
  // числовая ось
  line((-3.5, 0), (3.5, 0), mark: (end: ">"), stroke: 1pt)
  
  // точка -C
  line((-2, -0.1), (-2, 0.1), stroke: 1pt)
  content((-2, -0.4), [$-C$])
  
  // точка 0
  line((0, -0.1), (0, 0.1), stroke: 1pt)
  content((0, -0.4), [$0$])
  
  // точка C
  line((2, -0.1), (2, 0.1), stroke: 1pt)
  content((2, -0.4), [$C$])
})

Итог: если $-C <= x <= C $, то $|x| <= C$, чтд

= 6. Доказать, что $|a + b| <= |a| + |b|, "где" a, b in bb(R)$ <proof-6>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-6">see extra</a>

Пусть $C = |a| + |b| >= 0$. Тогда если $-C <= a + b <= C$, то $|a + b| <= C = |a| + |b|$

Очевидно, что $forall x in bb(R) -|x| <= x <= |x|$. Для $a$ и $b$:

$-|a| <= a <= |a|$\
и\
$-|b| <= b <= |b|$\
#line()
$-|a| - |b| <= a + b <= |a| + |b|$\
$ -C = -(|a| + |b|) <= a + b <= |a| + |b| = C$
#line()
$|a + b| <= C => |a + b| <= |a| + |b|$, чтд

= 7. Найти корни квадратного уравнения $a x^2 + b x + c = 0, 1 a != 0$ <proof-7>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-7">see extra</a>

$ a x^2 + b x + c = 0 | dot 1/a $
$ x^2 + b_a x + c_a = 0, b_a = b/a, c_a = c/a $
$ "Если" x_1 "и" x_2 "- корни, то" $
$ (x - x_1)(x - x_2) = 0 $
$ x^2 + x dot(-x_1 - x_2) + x_1 x_2 = 0 $
$ cases(
  a = 1,
  b_a = -x_1 - x_2,
  c_a = x_1 x_2
) $

Внезапно, теорема Виета:

$ cases(
  x_1 + x_2 = -b_a,
  x_1 x_2 = c_a
) $

Хорошо было бы найти $x_1 - x_2$

$ (x_1 + x_2)^2 = x_1^2 + 2 x_1 x_2 + x_2^2 $
$ (x_1 - x_2)^2 = x_1^2 - 2 x_1 x_2 + x_2^2 $
$ (x_1 - x_2)^2 = (x_1^2 + 2 x_1 x_2 + x_2^2) - 2x_1x_2 - 2x_1x_2 = (x_1 + x_2)^2 - 4x_1x_2 = b_a^2 - 4 c_a $
$ "Пусть" x_1 >= x_2 ", тогда" $
$ x_1 - x_2 = sqrt(b_a^2 - 4c_a) = sqrt(b^2/a^2 - (4c)/a) = sqrt((b^2 - 4 a c)/a^2) = sqrt(b^2 - 4 a c) / abs(a) $

$ "Пусть" a>0 ", тогда" x_1 - x_2 = sqrt(b^2 - 4 a c)/a $

$ cases(
  display(x_1 + x_2 = -b/a),
  display(x_1 - x_2 = sqrt(b^2 - 4 a c)/a)
) $
#line()
$ 2 x_1 = -b/a + sqrt(b^2 - 4 a c)/a = (-b + sqrt(b^2 - 4 a c))/a => x_1 = (-b + sqrt(b^2 - 4 a c))/(2a) $
$ x_2 = x_1 - sqrt(b^2 - 4 a c)/(a) = (-b + sqrt(b^2 - 4 a c))/(2a) - (2 sqrt(b^2 - 4 a c))/(2a) = (-b - sqrt(b^2 - 4 a c))/(2a) $

$ "Пусть" a < 0 ", тогда" x_1 - x_2 = -sqrt(b^2 - 4 a c)/a $
$ cases(
  display(x_1 + x_2 = -b/a),
  display(x_1 - x_2 = -sqrt(b^2 - 4 a c)/a)
) $
#line()
$ 2 x_1 = -b/a - sqrt(b^2 - 4 a c)/a = -(b + sqrt(b^2 - 4 a c))/a => x_1 = (-b - sqrt(b^2 - 4 a c))/(2a) $
$ x_2 = x_1 + sqrt(b^2 - 4 a c)/a = (-b - sqrt(b^2 - 4 a c))/(2a) + (2 sqrt(b^2 - 4 a c))/(2a) = (-b + sqrt(b^2 - 4 a c))/(2a) $

$ "Пусть" x_1 < x_2 ", тогда" -(x_1 - x_2) = sqrt(b^2 - 4 a c) / abs(a) "или" x_1 - x_2 = -sqrt(b^2 - 4 a c)/abs(a) $

Раскрытие модуля аналогично ранее рассмотрену и даст результат с противоположным знаком, поэтому итоговый ответ:

$ x_(12) = (-b + sqrt(b^2 - 4 a c))/(2a) "при условии существования" x_1 "и" x_2 ", чтд" $

= 8. Вывести формулы теоремы Виета для квадратного уравнения $a x^2 + b x + c = 0, 1 a != 0$ <proof-8>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-8">see extra</a>

$ a x^2 + b x + c = 0 | dot 1/a $
$ x^2 + b_a x + c_a = 0, b_a = b/a, c_a = c/a $
$ "Если" x_1 "и" x_2 "- корни, то" $
$ (x - x_1)(x - x_2) = 0 $
$ x^2 + x dot(-x_1 - x_2) + x_1 x_2 = 0 $
$ cases(
  a = 1,
  b_a = -x_1 - x_2,
  c_a = x_1 x_2
) $

Внезапно, теорема Виета, чтд

$ cases(
  display(x_1 + x_2 = -b_a),
  display(x_1 x_2 = c_a)
) "или" cases(
  display(x_1 + x_2 = -b/a),
  display(x_1 x_2 = c/a)
) $

= 9. Найти сумму первых $N$ натуральных чисел <proof-9>
// <a href="https://konsilerinos.github.io/books-knowledge/extra/maths/algebra/proofs.html#proof-9">see extra</a>

$ 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 = 10 + 10 dot (10/2 - 1) + 10/2 $
$ "Если " N=10 ", то" sum_(i=1..N) i = N + N^2/2  - N + N/2 = (N dot (N + 1))/2 $

Решение дилетантское и требует доказательства методом математической индукции например, пока что оставлю так
