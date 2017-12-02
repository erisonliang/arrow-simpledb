
#include <iostream>
#include <tables/DBTable.h>
#include "Tables.h"

using namespace std;

using arrow::DoubleBuilder;
using arrow::Int64Builder;
using arrow::ListBuilder;
using arrow::Status;
using arrow::RecordBatch;
using arrow::Table;
using arrow::Column;
using arrow::Field;
using arrow::ChunkedArray;
using arrow::ArrayVector;
using arrow::TableBatchReader;
using arrow::Int64Type;
using arrow::DictionaryBuilder;
using arrow::StringDictionaryBuilder;

arrow::Status
Tables::createSmallSimpleColumns(std::shared_ptr<DBTable>& table) {

    DBTable *pTable = new DBTable(
                {"id", "cost"},
                {arrow::int64(), arrow::float64()},
                {GenericColumnCursor::PLAIN, GenericColumnCursor::PLAIN}
            );

    table.reset(pTable);

    table->addRow({DBTable::int64(11), DBTable::float64(21.9)});
    table->addRow({DBTable::int64(12), DBTable::float64(22.9)});

    table->make();

    return Status::OK();
}

arrow::Status
Tables::createSmallSimpleStringColumns(std::shared_ptr<DBTable>& table)
{
    DBTable *pTable = new DBTable(
            {"foo", "bar"},
            {arrow::utf8(), arrow::utf8()},
            {GenericColumnCursor::PLAIN, GenericColumnCursor::PLAIN}
    );

    table.reset(pTable);

    table->addRow({DBTable::utf8("eleven"), DBTable::utf8("twenty one")});
    table->addRow({DBTable::utf8("twelve"), DBTable::utf8("twenty two")});

    table->make();

    return Status::OK();

}

arrow::Status
Tables::createSmallChunkedColumns(std::shared_ptr<DBTable>& table)
{
    DBTable *pTable = new DBTable(
            {"id", "cost"},
            {arrow::int64(), arrow::float64()},
            {GenericColumnCursor::PLAIN, GenericColumnCursor::PLAIN}
    );

    table.reset(pTable);

    table->addRow({DBTable::int64(11), DBTable::float64(21.9)});
    table->addRow({DBTable::int64(12), DBTable::float64(22.9)});

    table->endChunk();

    table->addRow({DBTable::int64(31), DBTable::float64(41.9)});
    table->addRow({DBTable::int64(32), DBTable::float64(42.9)});

    table->make();

    return Status::OK();
}

arrow::Status
Tables::createSimple(std::shared_ptr<DBTable>& table)
{
    DBTable *pTable = new DBTable(
            {"id", "cost"},
            {arrow::int64(), arrow::float64()},
            {GenericColumnCursor::PLAIN, GenericColumnCursor::PLAIN}
    );

    table.reset(pTable);

    for (int64_t i = 0; i < 100; i++) {
        table->addRow({DBTable::int64(i), DBTable::float64(0.5 * i)});
    }

    table->endChunk();

    for (int64_t i = 0; i < 100; i++) {
        table->addRow({DBTable::int64(i), DBTable::float64(0.5 * i)});
    }

    table->make();

    return Status::OK();
}

arrow::Status
Tables::createSmallDictionaryColumns(std::shared_ptr<arrow::Table>& table)
{

    arrow::MemoryPool* pool = arrow::default_memory_pool();

    std::vector<std::shared_ptr<arrow::Field>> schema_vector = {
            arrow::field("id", arrow::int64()),
            arrow::field("cost", arrow::int64())
    };
    auto schema = std::make_shared<arrow::Schema>(schema_vector);


    DictionaryBuilder<Int64Type> id_builder(pool);
    ARROW_RETURN_NOT_OK(id_builder.Append(11));
    ARROW_RETURN_NOT_OK(id_builder.Append(12));
    ARROW_RETURN_NOT_OK(id_builder.Append(11));
    ARROW_RETURN_NOT_OK(id_builder.Append(12));


    DictionaryBuilder<Int64Type> cost_builder(pool);
    ARROW_RETURN_NOT_OK(cost_builder.Append(23));
    ARROW_RETURN_NOT_OK(cost_builder.Append(23));
    ARROW_RETURN_NOT_OK(cost_builder.Append(25));
    ARROW_RETURN_NOT_OK(cost_builder.Append(25));

    std::shared_ptr<arrow::Array> id_array;
    ARROW_RETURN_NOT_OK(id_builder.Finish(&id_array));
    std::shared_ptr<arrow::Array> cost_array;
    ARROW_RETURN_NOT_OK(cost_builder.Finish(&cost_array));


    shared_ptr<Field> id_field = arrow::field("id", arrow::int64());
    shared_ptr<Field> cost_field = arrow::field("cost", arrow::int64());

    shared_ptr<Column> id_col = std::make_shared<Column>(id_field, id_array);
    shared_ptr<Column> cost_col = std::make_shared<Column>(cost_field, cost_array);

    std::vector<std::shared_ptr<Column>> columns = {id_col, cost_col};

    table.reset(new Table(schema, columns));

    /*
    std::shared_ptr<Column> c0 = table->column(0);
    std::cout << "Num chunks: " << c0->data()->num_chunks() << std::endl;
    std::cout << "Chunk 0 length: " << c0->data()->chunk(0)->length() << std::endl;
    std::shared_ptr<arrow::DictionaryArray> da = dynamic_pointer_cast<arrow::DictionaryArray>(c0->data()->chunk(0));
    std::cout << "dict array: " << da.get() << std::endl;

    std::cout << "dict array length: " << da->length() << std::endl;
    std::cout << "dict array dict length: " << da->dictionary()->length() << std::endl;
    std::cout << "dict array indices length: " << da->indices()->length() << std::endl;

    std::cout << "dict array dictionary type: " << da->dictionary()->type_id() << std::endl;

    std::cout << "dict array indices type: " << da->indices()->type_id() << std::endl;
     */


    return Status::OK();

}

arrow::Status
Tables::createSmallStringDictionaryColumns(std::shared_ptr<arrow::Table>& table)
{

    arrow::MemoryPool* pool = arrow::default_memory_pool();

    shared_ptr<Field> foo_field = arrow::field("foo", arrow::utf8());
    shared_ptr<Field> bar_field = arrow::field("bar", arrow::utf8());

    std::vector<std::shared_ptr<arrow::Field>> schema_vector =
            {foo_field, bar_field};
    auto schema = std::make_shared<arrow::Schema>(schema_vector);

    arrow::StringDictionaryBuilder foo_builder(pool);
    arrow::StringDictionaryBuilder bar_builder(pool);

    ARROW_RETURN_NOT_OK(foo_builder.Append("eleven"));
    ARROW_RETURN_NOT_OK(foo_builder.Append("twelve"));

    ARROW_RETURN_NOT_OK(bar_builder.Append("twenty one"));
    ARROW_RETURN_NOT_OK(bar_builder.Append("twenty two"));

    std::shared_ptr<arrow::Array> foo_array;
    ARROW_RETURN_NOT_OK(foo_builder.Finish(&foo_array));
    std::shared_ptr<arrow::Array> bar_array;
    ARROW_RETURN_NOT_OK(bar_builder.Finish(&bar_array));

    shared_ptr<Column> foo_col = std::make_shared<Column>(foo_field, foo_array);
    shared_ptr<Column> bar_col = std::make_shared<Column>(bar_field, bar_array);

    std::vector<std::shared_ptr<Column>> columns = {foo_col, bar_col};

    table.reset(new Table(schema, columns));

    return Status::OK();

}

arrow::Status
Tables::createChunkedDictionaryColumns(std::shared_ptr<arrow::Table>& table)
{
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    shared_ptr<Field> id_field = arrow::field("id", arrow::int64());
    shared_ptr<Field> cost_field = arrow::field("cost", arrow::utf8());
    std::vector<std::shared_ptr<arrow::Field>> schema_vector =
            {id_field, cost_field};
    auto schema = std::make_shared<arrow::Schema>(schema_vector);

    // first chunks

    DictionaryBuilder<Int64Type> id_builder_1(pool);
    StringDictionaryBuilder cost_builder_1(pool);

    ARROW_RETURN_NOT_OK(id_builder_1.Append(11));
    ARROW_RETURN_NOT_OK(id_builder_1.Append(12));

    ARROW_RETURN_NOT_OK(cost_builder_1.Append("twenty one"));
    ARROW_RETURN_NOT_OK(cost_builder_1.Append("twenty two"));

    std::shared_ptr<arrow::Array> id_array_1;
    ARROW_RETURN_NOT_OK(id_builder_1.Finish(&id_array_1));
    std::shared_ptr<arrow::Array> cost_array_1;
    ARROW_RETURN_NOT_OK(cost_builder_1.Finish(&cost_array_1));

    // second chunks

    DictionaryBuilder<Int64Type> id_builder_2(pool);
    StringDictionaryBuilder cost_builder_2(pool);

    ARROW_RETURN_NOT_OK(id_builder_2.Append(31));
    ARROW_RETURN_NOT_OK(id_builder_2.Append(32));
    ARROW_RETURN_NOT_OK(cost_builder_2.Append("forty one"));
    ARROW_RETURN_NOT_OK(cost_builder_2.Append("forty two"));

    std::shared_ptr<arrow::Array> id_array_2;
    ARROW_RETURN_NOT_OK(id_builder_2.Finish(&id_array_2));
    std::shared_ptr<arrow::Array> cost_array_2;
    ARROW_RETURN_NOT_OK(cost_builder_2.Finish(&cost_array_2));

    // make columns

    ArrayVector id_arrays({id_array_1, id_array_2});
    shared_ptr<ChunkedArray> id_chunked = std::make_shared<ChunkedArray>(id_arrays);

    ArrayVector cost_arrays({cost_array_1, cost_array_2});
    shared_ptr<ChunkedArray> cost_chunked = std::make_shared<ChunkedArray>(cost_arrays);

    shared_ptr<Column> id_col = std::make_shared<Column>(id_field, id_arrays);
    shared_ptr<Column> cost_col = std::make_shared<Column>(cost_field, cost_arrays);

    std::vector<std::shared_ptr<Column>> columns = {id_col, cost_col};

    table.reset(new Table(schema, columns));

    return Status::OK();
}


