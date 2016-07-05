# QPeerCastYP

![Platform](https://img.shields.io/badge/Platform-Mac%20OS%20X%20%7C%20Linux-blue.svg)
[![License](https://img.shields.io/badge/license-GPLv2-green.svg?label=License)](https://raw.githubusercontent.com/bookdraft/QPeerCastYP/master/LICENSE.txt)

QPeerCastYP は PeerCast の視聴を補助するアプリケーションです。

## Mac OS X で PeerCast を視聴するには

**Mono をインストールする**

[Mono](http://www.mono-project.com/) をダウンロードしてインストールします。このプログラムは、Mac OS X 上で PeerCastStation を動かすために必要です。ターミナルで下記のコマンドを実行してバージョン等が表示されればインストールできています。

    mono --version

**PeerCastStation を起動する**

[PeerCastStation (ZIP版)](http://www.pecastation.org/) をダウンロードして展開します。ターミナルに mono と入力し、引数に PeerCastStation.exe のパスを入力して実行します。パスの入力は、Finder から PeerCastStation.exe ファイルをターミナルにドラッグすると簡単にできます。

    mono /path/to/PeerCastStation.exe

GUI 無しで起動するので、ウェブ・ブラウザで http://localhost:7144/ を開いて正しく動作している事を確認します。

**メディア・プレイヤをインストールする**

[mpv](https://mpv.io/) をダウンロードしてインストールします。QPeerCastYP では、このプレイヤを使うように初期設定しています。[VLC](http://www.videolan.org/vlc/) で視聴することもできます。(プレイヤ設定例を参照)

**QPeerCastYP を起動して視聴する**

[QPeerCastYP](https://github.com/bookdraft/QPeerCastYP/releases) をダウンロードしてインストールします。あとは、起動して見たい配信者をクリックすれば視聴できます。

## プレイヤ設定例 (Mac OS X)

VLC で視聴したい場合は、**設定 > プレイヤ** で下記のように設定します。

**プレイヤ1**

プログラム: `open`

引数: `-a VLC -n "$STREAM_URL(mmsh)"`

種類: `wmv`

**プレイヤ2**

プログラム: `open`

引数: `-a VLC -n "$STREAM_URL"`

種類: `flv|mkv|webm`

## ビルド・インストール方法

**Mac OS X**

事前に、Xcode、Command Line Tools、[Homebrew](http://brew.sh/) をインストールして下さい。

    brew install qt
    ./configure
    make
    macdeployqt QPeerCastYP.app
    cp -r QPeerCastYP.app /Applications

**Ubuntu**

    sudo apt-get install build-essential libqt4-dev
    ./configure
    make
    sudo make install
