# 佐藤さんモジュールデータ取得テストプログラム sicat (素核研 千代浩司)

## 使いかた

コマンドラインから

```
sicat 192.168.0.16
```

のようにIPアドレスを一つ指定して使います。指定されたIPアドレ
スのモジュールに接続し、データ取得リクエストを数回(デフォル
トでは4回)出して、返事を読み、データを取得します。リクエスト
ごとに取得できたデータ数およびバイト数を標準エラー出力に表示
します。この表示をしないようにするには -q オプションを指定し
てください。

取得したデータはこのプログラムのプロセスの標準出力が端末だっ
た場合には保存されませんが、

```
sicat 192.168.0.16 > datafile
```

のようにファイルにリダイレクトされるとそのファイルに取得した
データをセーブします。

モジュールに接続できなかった場合は2秒でタイムアウトします。

## オプション

-s sleep

SiTCPモジュールに接続して最初のリクエストを出すまでsleep秒ス
リープします。-s 0.2のように小数で指定することもできます。

-n n_request

データ取得リクエストを何回出すか指定します。デフォルトでは4回
出します。0 も指定できてこれを指定するとTCPでコネクトしたあと
なにもせずに接続を切りますので接続できるかどうか調べることに
使えます。

-e n_event

何個のイベントのリクエストを出すかを指定します。デフォルトでは
4096個のイベントリクエストを出します。

以下動作例をもとに説明します。

IPアドレスだけを指定した場合:

```
kensdaqpc1% ./sicat 192.168.0.20
request #    1    24 events (   192 bytes )
request #    2  1144 events (  9152 bytes )
request #    3  1504 events ( 12032 bytes )
request #    4  1488 events ( 11904 bytes )
Total Events: 4160
Total Bytes:  33280 bytes
```

-s 0.1 で connectしてから0.1秒後に最初のリクエストを出す:

```
kensdaqpc1% ./sicat -s 0.1 192.168.0.20
sleeping 0.10 sec (by -s option)
request #    1  4088 events ( 32704 bytes )
request #    2  4088 events ( 32704 bytes )
request #    3  4088 events ( 32704 bytes )
request #    4  4088 events ( 32704 bytes )
Total Events: 16352
Total Bytes:  130816 bytes
```

シェルのリダイレクトでデータを保存する:

```
kensdaqpc1% ./sicat -n 10 192.168.0.20 > datafile
request #    1     8 events (    64 bytes )
request #    2  1304 events ( 10432 bytes )
request #    3  1445 events ( 11560 bytes )
request #    4  1539 events ( 12312 bytes )
request #    5  1432 events ( 11456 bytes )
request #    6  1544 events ( 12352 bytes )
request #    7  1432 events ( 11456 bytes )
request #    8  1542 events ( 12336 bytes )
request #    9  1436 events ( 11488 bytes )
request #   10  1546 events ( 12368 bytes )
Total Events: 13228
Total Bytes:  105824 bytes
kensdaqpc1% ls -l datafile
-rw-rw-r--  1 sendai sendai 105824 Sep 10 10:19 datafile
```

Total Bytes数と実際にファイルにセーブされたバイト数があっていることが
わかる。

(このプログラムには直接には関係しないが)データファイルの確認
にはhexdumpが使える:

```
kensdaqpc1% hexdump -Cv datafile| head
00000000  5a 10 ff ff ff 33 23 d3  5a 11 ff ff ff 33 03 cf  |Z....3#.Z....3..|
00000010  5a 12 ff ff ff 32 c3 cd  5a 13 ff ff ff 33 c3 cb  |Z....2..Z....3..|
00000020  5a 12 ff ff ff 2b 04 57  5a 13 ff ff ff 2b b4 55  |Z....+.WZ....+.U|
00000030  5a 14 ff ff ff 2b 74 59  5a 15 ff ff ff 2b 54 59  |Z....+tYZ....+TY|
00000040  5a 16 ff ff ff 2b 44 55  5a 17 ff ff ff 2b 84 58  |Z....+DUZ....+.X|
00000050  5a 10 ff ff ff 2b 54 5e  5a 11 ff ff ff 2b 44 59  |Z....+T^Z....+DY|
00000060  5a 10 ff ff ff 22 74 f3  5a 11 ff ff ff 22 84 ef  |Z...."t.Z...."..|
00000070  5a 12 ff ff ff 22 34 ed  5a 13 ff ff ff 22 e4 eb  |Z...."4.Z...."..|
00000080  5a 14 ff ff ff 22 a4 f0  5a 15 ff ff ff 22 94 ef  |Z...."..Z...."..|
00000090  5a 16 ff ff ff 22 94 ec  5a 17 ff ff ff 22 a4 ed  |Z...."..Z...."..|
```

あるいはいきなりhexdumpに流すこともできる(長くなるので-n 1で
1回だけリクエストを出した):

```
kensdaqpc1% ./sicat -n 1 192.168.0.20 | hexdump -Cv
request #    1     8 events (    64 bytes )
Total Events: 8
Total Bytes:  64 bytes
00000000  5a 24 ff ff ff 2a a4 6e  5a 25 ff ff ff 2a 74 6b  |Z$...*.nZ%...*tk|
00000010  5a 26 ff ff ff 2a f4 66  5a 27 ff ff ff 2a c4 66  |Z&...*.fZ'...*.f|
00000020  5a 20 ff ff ff 2a e4 5e  5a 21 ff ff ff 2a c4 64  |Z ...*.^Z!...*.d|
00000030  5a 22 ff ff ff 2a d4 67  5a 23 ff ff ff 2a f4 67  |Z"...*.gZ#...*.g|
00000040
```

イベントデータ数を指定する例:

```
kensdaqpc1% ./sicat -n 10 -e 100 192.168.0.20
request #    1     8 events (    64 bytes )
request #    2   100 events (   800 bytes )
request #    3   100 events (   800 bytes )
request #    4   100 events (   800 bytes )
request #    5   100 events (   800 bytes )
request #    6   100 events (   800 bytes )
request #    7   100 events (   800 bytes )
request #    8   100 events (   800 bytes )
request #    9   100 events (   800 bytes )
request #   10   100 events (   800 bytes )
Total Events: 908
Total Bytes:  7264 bytes
```

この例ではイベントデータ数として100個のデータを-n 10で10回リ
クエストしている。最初のリクエストの時点では100個データがな
かったので、ある分だけ(この例だと8イベント64バイト)データが
返って来ている。2回目以降は100イベント(800バイト)返って来て
いる。

タイムアウトの例:

```
kensdaqpc1% time ./sicat -e 100 192.168.0.1
connection fail: 192.168.0.1
./sicat -e 100 192.168.0.1  0.00s user 0.00s system 0% cpu 2.000 total
```

存在しないモジュール192.168.0.1へリクエストを出した例。time
で実行時間を計っているので2秒後に終了していることがわかる。
