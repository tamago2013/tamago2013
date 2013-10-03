/*

    ls-accumulatorで使うリングバッファクラスの定義

    2013-10-01
    MTM

*/

#ifndef LS_ACCUMULATOR_RING_BUFFER_HPP_
#define LS_ACCUMULATOR_RING_BUFFER_HPP_

#include<vector>

namespace ls_accumulator
{

    //リングバッファクラス
    //書き込み回数がデータサイズを超えると前に戻って書き始める。
    //テンプレートは<データ型,データ最大サイズ>。
    template < class DataType > class ring_buffer
    {

        public:

        //-------------------------------
        //constructor,destructor
        //-------------------------------

        ring_buffer():
            _data( 0 ),
            _max_size( 1 ),
            _next_writing_pos( 0 )
        {
        }

        virtual ~ring_buffer(){}

        //-------------------------------
        //operator
        //-------------------------------
        //[]オペレータ：_dataの参照を返す
        DataType& operator[]( size_t indice )
        {
            return _data[ indice ];
        }

        //-------------------------------
        //setter
        //-------------------------------
        //push_back:リングバッファにデータを追加する
        void push_back( DataType& new_data )
        {
            if( _data.size() < _max_size ) //まだリングバッファを1週していないときは単純に追加
            {
                _data.push_back( new_data );
            }
            else //1週してからは、前のデータを置き換える
            {
                _data[ _next_writing_pos ] = new_data;
            }

            _next_writing_pos = ( _next_writing_pos + 1 ) % _max_size; //次の書き込み位置を計算
        }

        //set_max_size：最大サイズをセットする
        void set_max_size( size_t max )
        {
            _max_size = max;

            if( _max_size == 0 )
            {
                _max_size = 1;
            }

            if( _data.size() > _max_size )
            {
                _data.resize( _max_size );
            }
            if( _next_writing_pos >= _max_size )
            {
                _next_writing_pos = 0;
            }
        }

        //-------------------------------
        //getter
        //-------------------------------
        //size：書きこまれたデータサイズを返す。リングバッファを1週して以降はリングバッファの最大サイズが返される。
        size_t size() const
        {
            return _data.size();
        }

        //max_size：リングバッファの最大サイズが返される
        size_t max_size() const
        {
            return _max_size;
        }

        private:

        //--------------------------------
        //data
        //-------------------------------
        std::vector< DataType > _data;      //データの実体
        size_t _max_size;                   //リングバッファが1週する最大サイズ
        size_t _next_writing_pos;           //次に書きこむポジション

    };



}

#endif // LS_ACCUMULATOR_RING_BUFFER_HPP_
