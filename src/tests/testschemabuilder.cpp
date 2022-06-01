#include "tests/testschemabuilder.hpp"

#include <QDebug>
#include <QElapsedTimer>

#include <orm/db.hpp>
#include <orm/exceptions/runtimeerror.hpp>
#include <orm/schema.hpp>

#include "models/user.hpp"

#include "configuration.hpp"

// clazy:excludeall=unused-non-trivial-variable

using Orm::Constants::ID;
using Orm::Constants::MyISAM;
using Orm::Constants::NAME;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8Generalci;
using Orm::Constants::UTF8Unicodeci;
using Orm::Constants::NotImplemented;
using Orm::Exceptions::RuntimeError;

using Orm::DB;
using Orm::Schema;
using Orm::SchemaNs::Blueprint;

using Orm::SchemaNs::Constants::Cascade;
using Orm::SchemaNs::Constants::Restrict;

namespace TinyPlay::Tests
{

void TestSchemaBuilder::run() const
{
    QElapsedTimer timer;
    timer.start();

    resetAllQueryLogCounters();

    // For current connection
    const auto driverName = DB::driverName();

    qInfo().nospace()
            << "\n\n================="
            << "\n  SchemaBuilder  "
            << "\n=================";

    /* Schema create and drop database */
    {
        qInfo() << "\n\nSchema create and drop database\n---";

        Schema::createDatabase("tinyplay_schema_database");
        Schema::dropDatabaseIfExists("tinyplay_schema_database");

        qt_noop();
    }

    /* Schema modifiers and dropColumns */
    {
        qInfo() << "\n\nSchema modifiers and dropColumns\n---";

        Schema::create("schema_modifiers", [&driverName](Blueprint &table)
        {
//                table.id().startingValue(10);
            table.bigInteger("id").autoIncrement().isUnsigned().startingValue(5);
            table.string("name").defaultValue("guest");
            table.string("name1").nullable();
            table.string("name2").comment("name2 note");
            table.string("name3");
            table.string("name4").invisible();
            table.string("name5").charset(UTF8);
            table.string("name6");
            table.string("name7").charset(UTF8);
            if (driverName == QMYSQL) {
                table.string("name8").collation(UTF8Unicodeci);
                table.string("name9").charset(UTF8).collation(UTF8Unicodeci);
            }
            table.bigInteger("big_int").isUnsigned();
            table.bigInteger("big_int1");
        });

        // Test after and first
        Schema::table("schema_modifiers", [](Blueprint &table)
        {
            table.after(ID, [](Blueprint &table_)
            {
                table_.string("string_after_1");
                table_.integer("integer_after_1");
            });

            table.after("string_after_1", [](Blueprint &table_)
            {
                table_.string("string_after_2");
                table_.integer("integer_after_2");
            });

            table.string("string_after_3").after("integer_after_1");

            table.string("first").first();
        });

        Schema::dropColumns("schema_modifiers", {"name2", "name1"});
        Schema::dropColumn("schema_modifiers", "name");
        Schema::dropColumns("schema_modifiers", "name5", "name6", "name7");

        Schema::dropIfExists("schema_modifiers");

        qt_noop();
    }

    /* Schema defaultValue modifier with Expression */
    {
        qInfo() << "\n\nSchema defaultValue modifier with Expression\n---";

        Schema::create("schema_modifier_defaultValue_expr", [](Blueprint &table)
        {
            table.string(NAME).defaultValue("guest");
            table.string("name_raw").defaultValue(DB::raw("'guest_raw'"));
        });

        Schema::dropIfExists("schema_modifier_defaultValue_expr");

        qt_noop();
    }

    /* Schema defaultValue modifier with boolean */
    {
        qInfo() << "\n\nSchema defaultValue modifier with boolean\n---";

        Schema::create("schema_modifier_defaultValue_bool", [](Blueprint &table)
        {
            table.boolean("boolean");
            table.boolean("boolean_false").defaultValue(false);
            table.boolean("boolean_true").defaultValue(true);
            table.boolean("boolean_0").defaultValue(0);
            table.boolean("boolean_1").defaultValue(1);
        });

        Schema::dropIfExists("schema_modifier_defaultValue_bool");

        qt_noop();
    }

    /* Schema escaping in the defaultValue modifier */
    {
        qInfo() << "\n\nSchema escaping in the defaultValue modifier\n---";

        Schema::create("schema_modifier_defaultValue_escaping", [](Blueprint &table)
        {
            // String contains \t after tab word
            table.string("string").defaultValue(R"(Text ' and " or \ newline
and tab	end)");
        });

        Schema::dropIfExists("schema_modifier_defaultValue_escaping");

        qt_noop();
    }

    /* Schema useCurrent/OnUpdate modifiers */
    {
        qInfo() << "\n\nSchema useCurrent/OnUpdate modifiers\n---";

        Schema::create("schema_usecurrent", [](Blueprint &table)
        {
            table.id();

            table.dateTime("created");
            table.dateTime("created_current").useCurrent();

            table.timestamp("created_t");
            table.timestamp("created_t_current").useCurrent();

            table.dateTime("updated");
            table.dateTime("updated_current").useCurrentOnUpdate();

            table.timestamp("updated_t");
            table.timestamp("updated_t_current").useCurrentOnUpdate();
        });

        Schema::dropIfExists("schema_usecurrent");

        qt_noop();
    }

    /* Schema types and after, first modifiers */
    {
        qInfo() << "\n\nSchema types and after, first modifiers\n---";

        Schema::create("schema_types_after_first", [&driverName](Blueprint &table)
        {
            // PostgreSQL supports temporary only
            if (driverName == QMYSQL) {
                table.charset = UTF8;
                table.collation = UTF8Generalci;
                table.engine = MyISAM;
            }
            table.temporary();

            table.id().from(15);
            table.Char("char");
            table.Char("char_10", 10);
            table.string("string");
            table.string("string_22", 22);
            table.tinyText("tiny_text");
            table.text("text");
            table.mediumText("medium_text");
            table.longText("long_text");

            table.integer("integer");
            table.tinyInteger("tinyInteger");
            table.smallInteger("smallInteger");
            table.mediumInteger("mediumInteger");
            table.bigInteger("bigInteger");

            table.unsignedInteger("unsignedInteger");
            table.unsignedTinyInteger("unsignedTinyInteger");
            table.unsignedSmallInteger("unsignedSmallInteger");
            table.unsignedMediumInteger("unsignedMediumInteger");
            table.unsignedBigInteger("unsignedBigInteger");

            table.unsignedDecimal("money", 10, 2);

            table.lineString("linestring1");
            table.lineString("linestring2").isGeometry();
            table.point("point1");
            table.point("point2", 3200).isGeometry();
            table.point("point3").isGeometry().srid(3400);
        });

        Schema::dropIfExists("schema_types_after_first");

        qt_noop();
    }

    /* Schema increment types */
    {
        qInfo() << "\n\nSchema increment types\n---";

        Schema::create("schema_increments", [](Blueprint &table)
        {
            table.increments("i");
        });
        Schema::dropIfExists("schema_increments");

        Schema::create("schema_increments", [](Blueprint &table)
        {
            table.integerIncrements("ii");
        });
        Schema::dropIfExists("schema_increments");

        Schema::create("schema_increments", [](Blueprint &table)
        {
            table.tinyIncrements("ti");
        });
        Schema::dropIfExists("schema_increments");

        Schema::create("schema_increments", [](Blueprint &table)
        {
            table.smallIncrements("si");
        });
        Schema::dropIfExists("schema_increments");

        Schema::create("schema_increments", [](Blueprint &table)
        {
            table.mediumIncrements("mi");
        });
        Schema::dropIfExists("schema_increments");

        Schema::create("schema_increments", [](Blueprint &table)
        {
            table.bigIncrements("bi");
        });
        Schema::dropIfExists("schema_increments");

        qt_noop();
    }

    /* Schema create/drop timestamps/rememberToken */
    {
        qInfo() << "\n\nSchema create/drop timestamps/rememberToken\n---";

        Schema::create("schema_timestamps_remembertoken", [](Blueprint &table)
        {
            table.id();

            table.timestamps();
            table.rememberToken();
        });

        Schema::table("schema_timestamps_remembertoken", [](Blueprint &table)
        {
            table.dropTimestamps();
            table.dropRememberToken();
        });

        Schema::create("schema_timestamps_precision", [](Blueprint &table)
        {
            table.id();

            table.timestamps(3);
        });

        Schema::dropIfExists("schema_timestamps_remembertoken");
        Schema::dropIfExists("schema_timestamps_precision");

        qt_noop();
    }

    /* Schema indexes */
    {
        qInfo() << "\n\nSchema indexes\n---";

        // Fluent indexes
        Schema::create("schema_indexes_fluent", [](Blueprint &table)
        {
            table.id();

            table.string("name_u").unique();

            table.string("name_i").index();
            table.string("name_i_cn").index("name_i_cn_index");

            table.string("name_f").fulltext();
            table.string("name_f_cn").fulltext("name_f_cn_fulltext");

            table.geometry("coordinates_s").spatialIndex();
            table.geometry("coordinates_s_cn")
                    .spatialIndex("coordinates_s_cn_spatial");
        });

        Schema::dropIfExists("schema_indexes_fluent");

        // Blueprint indexes
        Schema::create("schema_indexes_blueprint", [&driverName](Blueprint &table)
        {
            table.id();

            table.string("name_u");
            table.unique({"name_u"}, "name_u_unique");

            table.string("name_i");
            table.index({"name_i"});

            table.string("name_i_cn");
            table.index("name_i_cn", "name_i_cn_index");

            table.string("name_r");
            table.string("name_r1");
            if (driverName == QMYSQL)
                table.rawIndex(DB::raw("`name_r`, name_r1"), "name_r_raw");
            else if (driverName == QPSQL)
                table.rawIndex(DB::raw(R"("name_r", name_r1)"), "name_r_raw");
            else
                throw RuntimeError(NotImplemented);

            table.string("name_f");
            table.fullText({"name_f"});

            table.string("name_f_cn");
            table.fullText("name_f_cn", "name_f_cn_fulltext");

            table.geometry("coordinates_s");
            table.spatialIndex("coordinates_s");

            table.geometry("coordinates_s_cn");
            table.spatialIndex("coordinates_s_cn", "coordinates_s_cn_spatial");
        });

        Schema::dropIfExists("schema_indexes_blueprint");

        qt_noop();
    }

    /* Schema rename index */
    {
        qInfo() << "\n\nSchema rename index\n---";

        Schema::create("schema_rename_index", [](Blueprint &table)
        {
            table.id();

            table.string(NAME).unique();
        });

        Schema::table("schema_rename_index", [](Blueprint &table)
        {
            table.renameIndex("schema_rename_index_name_unique",
                              "schema_rename_index_name_unique_renamed");
        });

        Schema::dropIfExists("schema_rename_index");

        qt_noop();
    }

    /* Schema drop index by index name */
    {
        qInfo() << "\n\nSchema drop index by index name\n---";

        Schema::create("schema_dropindex_byname", [](Blueprint &table)
        {
            table.unsignedInteger(ID);
            table.primary(ID);

            table.string("name_u").unique();
            table.string("name_i").index();
            table.string("name_f").fulltext();
            table.geometry("coordinates_s").spatialIndex();
        });

        Schema::table("schema_dropindex_byname", [](Blueprint &table)
        {
            table.dropPrimary();
            table.dropUnique("schema_dropindex_byname_name_u_unique");
            table.dropIndex("schema_dropindex_byname_name_i_index");
            table.dropFullText("schema_dropindex_byname_name_f_fulltext");
            table.dropSpatialIndex("schema_dropindex_byname_coordinates_s_spatialindex");
        });

        Schema::dropIfExists("schema_dropindex_byname");

        qt_noop();
    }

    /* Schema drop index by column name */
    {
        qInfo() << "\n\nSchema drop index by column name\n---";

        Schema::create("schema_dropindex_bycolumn", [](Blueprint &table)
        {
            table.unsignedInteger(ID);
            table.primary(ID);

            table.string("name_u").unique();
            table.string("name_i").index();
            table.string("name_f").fulltext();
            table.geometry("coordinates_s").spatialIndex();
        });

        Schema::table("schema_dropindex_bycolumn", [](Blueprint &table)
        {
            table.dropPrimary();
            table.dropUnique({"name_u"});
            table.dropIndex({"name_i"});
            table.dropFullText({"name_f"});
            table.dropSpatialIndex({"coordinates_s"});
        });

        Schema::dropIfExists("schema_dropindex_bycolumn");

        qt_noop();
    }

    /* Schema drop index by multiple column names */
    {
        qInfo() << "\n\nSchema drop index by multiple column names\n---";

        Schema::create("schema_dropindex_bymorecolumns", [](Blueprint &table)
        {
            table.unsignedInteger(ID);
            table.unsignedInteger("id1");
            table.primary({ID, "id1"});

            table.string("name_u");
            table.string("name_u1");
            table.unique({"name_u", "name_u1"});

            table.string("name_i");
            table.string("name_i1");
            table.index({"name_i", "name_i1"});

            table.string("name_f");
            table.string("name_f1");
            table.fullText({"name_f", "name_f1"});
        });

        Schema::table("schema_dropindex_bymorecolumns", [](Blueprint &table)
        {
            table.dropPrimary({ID, "id1"});
            table.dropUnique({"name_u", "name_u1"});
            table.dropIndex({"name_i", "name_i1"});
            table.dropFullText({"name_f", "name_f1"});
        });

        Schema::dropIfExists("schema_dropindex_bymorecolumns");

        qt_noop();
    }

    /* Schema modify table and rename column */
    {
        qInfo() << "\n\nSchema modify table and rename column\n---";

        Schema::create("schema_modify", [](Blueprint &table)
        {
            table.id();
            table.string(NAME);
        });

        Schema::table("schema_modify", [](Blueprint &table)
        {
            table.Char("char");
            table.Char("char_10", 10);
            table.string("string");
            table.string("string_22", 22);
            table.tinyText("tiny_text");
            table.text("text");
            table.mediumText("medium_text");
            table.longText("long_text");

            table.integer("integer").nullable();
            table.tinyInteger("tinyInteger");
            table.smallInteger("smallInteger");
            table.mediumInteger("mediumInteger");

            table.dropColumn("long_text");
            table.dropColumns({"medium_text", "text"});
            table.dropColumns("smallInteger", "mediumInteger");

            // Rename column
            table.renameColumn("integer", "integer_renamed");
            table.renameColumn("string_22", "string_22_renamed");
        });

        // Rename column
        Schema::renameColumn("schema_modify", "char_10", "char_10_renamed");

        Schema::dropIfExists("schema_modify");

        qt_noop();
    }

    /* Schema foreign key constraints */
    {
        qInfo() << "\n\nSchema foreign key constraints\n---";

        Schema::create("schema_foreign", [](Blueprint &table)
        {
            table.id();

            table.unsignedBigInteger("user_id");
            table.unsignedBigInteger("torrent_id");
            table.unsignedBigInteger("role_id").nullable();

            table.foreign("user_id").references(ID).on("users")
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreign("torrent_id").references(ID).on("torrents")
                    .restrictOnDelete().restrictOnUpdate();
            table.foreign("role_id").references(ID).on("roles")
                    .nullOnDelete().cascadeOnUpdate();
        });

        Schema::dropIfExists("schema_foreign");

        qt_noop();
    }

    /* Schema foreign key constraints terser syntax */
    {
        qInfo() << "\n\nSchema foreign key constraints terser syntax\n---";

        Schema::create("schema_foreign_terser", [](Blueprint &table)
        {
            table.id();

            table.foreignId("user_id").constrained()
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreignId("torrent_id").constrained()
                    .restrictOnDelete().restrictOnUpdate();
            table.foreignId("role_id").nullable().constrained()
                    .nullOnDelete().cascadeOnUpdate();

            table.dropForeign({"user_id"});
            table.dropForeign("schema_foreign_terser_torrent_id_foreign");
            table.dropConstrainedForeignId("role_id");
        });

        Schema::dropIfExists("schema_foreign_terser");

        qt_noop();
    }

    /* Schema foreign key from the Model */
    {
        qInfo() << "\n\nSchema foreign key from the Model\n---";

        Models::Torrent torrent;
        Models::User user;

        Schema::create("schema_foreign_model", [&torrent, &user](Blueprint &table)
        {
            table.id();

            table.foreignIdFor(torrent).constrained()
                    .onDelete(Cascade).onUpdate(Restrict);
            table.foreignIdFor(user).nullable().constrained()
                    .nullOnDelete().cascadeOnUpdate();
        });

        Schema::dropIfExists("schema_foreign_model");

        qt_noop();
    }

    /* Schema rename table */
    {
        qInfo() << "\n\nSchema rename table\n---";

        Models::Torrent torrent;
        Models::User user;

        Schema::create("schema_rename_table", [](Blueprint &table)
        {
            table.id();
            table.string(NAME);
        });

        Schema::rename("schema_rename_table", "schema_rename_table_renamed");

        Schema::dropIfExists("schema_rename_table");
        Schema::dropIfExists("schema_rename_table_renamed");

        qt_noop();
    }

    logQueryCounters(timer.elapsed());
}

} // namespace TinyPlay::Tests
