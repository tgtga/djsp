#!/usr/bin/env rspec

require "tempfile"
require "./djsp"

ONESHOT_TESTS = {
  [ 2,     1] =>   0,
  [ 2,    25] =>  11,
  [ 2, 78901] => 258,
  [ 3,    13] =>   5,
  [ 3,    29] =>   5,
  [ 3,    32] =>   5,
  [ 3,    44] =>   7,
  [ 4,    19] =>   4,
  [ 4,    23] =>   4,
  [ 9,    44] =>   5,
  [10,    89] =>   5,
  [11,    32] =>   6
}

# SEQUENCE_TESTS = {
#   [ 2, (1..1000) ] => 
# }

RSpec.describe DJSP do
  context "logging" do
    Tempfile.new "djsp-tester" do |tempfile|
      DJSP.log = tempfile
    ensure
      DJSP.log&.close
      DJSP.log&.unlink
      DJSP.log = nil
    end
  end

  context "oneshot" do
    context "2-ary" do
      ONESHOT_TESTS.filter {|(base, _), _| base == 2 }.each do |(base, seed), expected|
        it("A(#{seed}) = #{expected}") { expect(DJSP.oneshot seed).to eq expected }
      end
    end
    context "n-ary" do
      ONESHOT_TESTS.each do |(base, seed), expected|
        it("A_#{base}(#{seed}) = #{expected}") { expect(DJSP.oneshot seed, base: base).to eq expected }
      end
    end
  end

  context "sequence" do
    it "only takes arithmetic sequences" do
      expect { DJSP.sequence ?a..?z }.to raise_error TypeError
      expect { DJSP.sequence nil.. }.to raise_error TypeError
    end
    it "only accepts known optimizations" do
      expect { DJSP.sequence optimize: :nonexistent }.to raise_error ArgumentError
      expect { DJSP.sequence optimize: "nonexistent" }.to raise_error ArgumentError
      expect { DJSP.sequence optimize: %q(non existent) }.to raise_error ArgumentError
    end
    it "should fail on unimplemented optimizations" do
      expect { DJSP.sequence base: 3, optimize: :root }.to raise_error RangeError
    end

    # context "2-ary" do
    #   it ""
    # end
  end
end
