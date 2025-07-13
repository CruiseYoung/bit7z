// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * bit7z - A C++ static library to interface with the 7-zip shared libraries.
 * Copyright (c) 2014-2022 Riccardo Ostani - All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch.hpp>

#include "utils/filesystem.hpp"
#include "utils/format.hpp"
#include "utils/shared_lib.hpp"

#include <bit7z/bitformat.hpp>
#include <bitarchivereader.hpp>
#include <bit7z/bitarchivewriter.hpp>

using namespace bit7z;
using namespace bit7z::test;
using namespace bit7z::test::filesystem;
using bit7z::BitArchiveWriter;


TEST_CASE( "BitArchiveWriter: TODO", "[bitarchivewriter]" ) {
    const BitArchiveWriter writer{ test::sevenzip_lib(), BitFormat::SevenZip };
    REQUIRE( writer.compressionFormat() == BitFormat::SevenZip );
}

#ifndef BIT7Z_USE_SYSTEM_CODEPAGE

TEST_CASE( "BitArchiveWriter: Creating an archive containing files with Unicode names", "[bitarchivewriter]" ) {
    static const TestDirectory testDir{ test_filesystem_dir };

    const auto testFormat = GENERATE( as< TestOutputFormat >(),
                                      TestOutputFormat{ "7z", BitFormat::SevenZip },
                                      TestOutputFormat{ "tar", BitFormat::Tar },
                                      TestOutputFormat{ "wim", BitFormat::Wim },
                                      TestOutputFormat{ "zip", BitFormat::Zip } );

    DYNAMIC_SECTION( "Archive format: " << testFormat.extension ) {
        constexpr auto renamedName = BIT7Z_STRING( "𤭢.svg" );

        BitArchiveWriter writer{ test::sevenzip_lib(), testFormat.format };
        REQUIRE_NOTHROW( writer.addFile( greek.name ) );
        REQUIRE_NOTHROW( writer.addFile( italy.name, renamedName ) ); // U+24B62

        buffer_t outBuffer;
        REQUIRE_NOTHROW( writer.compressTo( outBuffer ) );

        const BitArchiveReader reader{ test::sevenzip_lib(), outBuffer, testFormat.format };
        const auto greekItem = reader.find( greek.name );
        REQUIRE( greekItem != reader.cend() );
        REQUIRE( greekItem->name() == greek.name );
        REQUIRE( greekItem->path() == greek.name );

        const auto renamedItem = reader.find( renamedName );
        REQUIRE( renamedItem != reader.cend() );
        REQUIRE( renamedItem->name() == renamedName );
        REQUIRE( renamedItem->path() == renamedName );
    }
}

#endif