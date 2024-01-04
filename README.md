[![Build](https://github.com/toppers/hakoniwa-core-cpp-client/actions/workflows/build.yml/badge.svg)](https://github.com/toppers/hakoniwa-core-cpp-client/actions/workflows/build.yml)

このリポジトリは、C/C++で開発された箱庭コア機能([hakoniwa-core-cpp](https://github.com/toppers/hakoniwa-core-cpp/tree/main))を以下の３つの機能分類で、C API として提供することを目的としています。

1. 箱庭アセット
2. 箱庭コマンド
3. [箱庭コンダクタ](https://github.com/toppers/hakoniwa-conductor)

![スクリーンショット 2024-01-04 14 04 47](https://github.com/toppers/hakoniwa-core-cpp-client/assets/164193/80d2d524-c115-486a-8b99-5da2b83a6fa8)


# 箱庭アセット向け API

## 箱庭アセットのユースケース

箱庭アセットのシミュレーションの基本処理フローは以下であり、1-3が箱庭アセットとしての基本ユースケースになります。

1. 箱庭アセットを登録する **[UC-HAKO-ASSET-1]**
2. 箱庭コア機能に対してシミュレーション実行を依頼する **[UC-HAKO-ASSET-2]**
3. 箱庭コア機能からのイベント（コールバック）として、箱庭アセットの処理を実行する **[UC-HAKO-ASSET-3]**
4. シミュレーション終了まで 2-3 を繰り返す

ここで、**[UC-HAKO-ASSET-3]** の処理は、以下の３種類に分類されます。
それぞれ、シミュレーションの必要なタイミングで、箱庭コア機能からコールバックされます。

* 箱庭アセットの初期化処理 **[UC-HAKO-ASSET-3-a]**
  * 箱庭のシミュレーション開始時点で１回だけ呼び出されます。
* 箱庭アセットのシミュレーション実行処理 **[UC-HAKO-ASSET-3-b]**
  * 箱庭のシミュレーション時間ステップ毎に呼び出されます。
* 箱庭アセットのリセット処理 **[UC-HAKO-ASSET-3-c]**
  * 箱庭のシミュレーションがリセットされたタイミングで呼び出されます。

また、箱庭アセットは、必要に応じて、以下の箱庭コア機能を利用できます。

* 箱庭シミュレーション時間を取得する **[UC-HAKO-ASSET-4]**
* 箱庭PDUデータを読み込みする **[UC-HAKO-ASSET-5]**
* 箱庭PDUデータを書き込みする **[UC-HAKO-ASSET-6]**

<img width="674" alt="スクリーンショット 2024-01-04 14 36 01" src="https://github.com/toppers/hakoniwa-core-cpp-client/assets/164193/4b690689-d5ac-4eba-82cd-69dafb4fdf50">



## 箱庭 API 仕様

### 箱庭アセットを登録する [UC-HAKO-ASSET-1]


**関数名**: `hako_asset_register`

**目的**:  
箱庭シミュレーション環境内で使用されるアセットを登録し、コールバック関数を関連付けます。

**引数**:  
- `asset_name`: 登録するアセットの名前。アセットを一意に識別するために使用されます。型: `const char*`
- `config_path`: アセットの設定ファイルへのパス。アセットの初期化や設定に必要な情報が含まれることを想定します。型: `const char*`
- `callback`: アセットのイベント処理を行うコールバック関数へのポインタ。このコールバックは、アセットの初期化、シミュレーションステップ、リセットなどのイベントが発生したときに呼び出されます。型: `hako_asset_callback_t*`
- `delta_usec`: シミュレーション時間のタイムステップをマイクロ秒単位で指定します。この値はシミュレーションが更新される頻度を決定します。型: `hako_time_t`

**戻り値**:  
成功時は `0` を返します。失敗時は非 `0` のエラーコードを返します。

**エラーハンドリング**:  
- `asset_name` が NULL または空文字列の場合、`EINVAL`（不正な引数）エラーコードを返します。
- `config_path` が不正なパスであるか、指定された設定ファイルが存在しない場合、`ENOENT`（ファイルが存在しない）エラーコードを返します。
- `callback` が NULL の場合、`EINVAL` エラーコードを返します。
- `delta_usec` が許容される範囲外の場合、`ERANGE`（数値の範囲外）エラーコードを返します。

**使用例**:

```c
#include <errno.h>
#include "hako_asset.h"

// コールバック関数の実装
void my_asset_callback(hako_event_t event, void* user_data) {
    // イベントタイプに応じた処理をここに記述する
}

int main() {
    const char* asset_name = "my_asset";
    const char* config_path = "/path/to/config.json";

    // コールバック関数と時間ステップを指定してアセットを登録
    int result = hako_asset_register(asset_name, config_path, my_asset_callback, 1000000); // 1秒ごとに更新

    if (result != 0) {
        // エラーハンドリング
        printf("Error: %s\n", strerror(errno));
    }

    // シミュレーションの実行など、他の処理を続ける
    return 0;
}
```


### 箱庭コア機能に対してシミュレーション実行を依頼する [UC-HAKO-ASSET-2]

TODO

### 箱庭コア機能からのイベント（コールバック）として、箱庭アセットの処理を実行する [UC-HAKO-ASSET-3]
TODO

#### 箱庭アセットの初期化処理 [UC-HAKO-ASSET-3-a]
TODO

#### 箱庭アセットのシミュレーション実行処理 [UC-HAKO-ASSET-3-b]
TODO

#### 箱庭アセットのリセット処理 [UC-HAKO-ASSET-3-c]
TODO

### 箱庭シミュレーション時間を取得する [UC-HAKO-ASSET-4]
TODO

### 箱庭PDUデータを読み込みする [UC-HAKO-ASSET-5]
TODO

### 箱庭PDUデータを書き込みする [UC-HAKO-ASSET-6]
TODO


## サンプルコード

TODO

## 動作確認手順

TODO

# 箱庭コマンド向け API

TODO(コントリビュート募集中)

# 箱庭コンダクタ向け API 

TODO(コントリビュート募集中)
