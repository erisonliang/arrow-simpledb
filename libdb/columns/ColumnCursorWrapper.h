

#ifndef COLUMN_CURSOR_WRAPPER_H
#define COLUMN_CURSOR_WRAPPER_H

#include "tables/ScanTableCursor.h"
#include "GenericColumnCursor.h"
#include "BaseColumnCursor.h"
#include "columns/DBSchema.h"

namespace db {

    class TableCursor;

/**
 * This cursor is actually used for executing queries. Notice it only has the
 * most minimal methods for cursor positioning, since it gets its position from the
 * table cursor it belongs to.
 *
 * @tparam T The underlying Arrow element type for the array, :: for example, arrow::Int64Type.
 */
    template<typename T>
    class ColumnCursorWrapper : public GenericColumnCursor {
    public:

        explicit ColumnCursorWrapper(
                std::shared_ptr<arrow::Column> column,
                db::ColumnEncoding encoding,
                TableCursor &table_cursor);

        explicit ColumnCursorWrapper(
                std::shared_ptr<BaseColumnCursor<T>> base_cursor,
                TableCursor &table_cursor);

        bool isNull() override;

        typename T::ElementType get();

        void reset() override;

    private:
        std::shared_ptr<BaseColumnCursor<T>> _base_cursor;

    };

};

#endif // COLUMN_CURSOR_WRAPPER_H
