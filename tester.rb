#!/usr/bin/env rspec

require "tempfile"
require "./djsp"

ONESHOT_TESTS = {
   2 => {
        1 =>   0,
       25 =>  11,
    78901 => 258
  },
   3 => {
       13 =>   5,
       29 =>   5,
       32 =>   5,
       44 =>   7
  },
   4 => {
       19 =>   4,
       23 =>   4
  },
   9 => { 44 => 5 },
  10 => { 89 => 5 },
  11 => { 32 => 6 }
}

SEQUENCE_TESTS = {
  2 => {
    (1..100) => { 2 => 1, 3 => 6, 9 => 7, 19 => 9, 25 => 11, 37 => 17, 77 => 19 }
  }
}

# SEQUENCE_TESTS = {
#   [ 2, (1..1000) ] => 
# }

RSpec.describe DJSP do
  context "logging" do
    it("should log to an open log") do
      Tempfile.create "djsp-tester" do |tempfile|
        DJSP.log = tempfile
        DJSP.message "hello, world!"
        expect(File.new(tempfile).read).to match /\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}.\d{3}[+-]\d{4}: hello, world!/
      end
    end

    it("should not log to an empty log") do
      Tempfile.create "djsp-tester" do |tempfile|
        DJSP.log = nil
        DJSP.message "hello, world!"
        expect(File.new(tempfile).read).to eq ""
      end
    end
  end

  context "oneshot" do
    context "2-ary" do
      ONESHOT_TESTS[2].each do |seed, expected|
        it("A(#{seed}) = #{expected}") { expect(DJSP.oneshot seed).to eq expected }
      end
    end
    context "n-ary" do
      ONESHOT_TESTS.each do |base, hash|
        hash.each do |seed, expected|
          it("A_#{base}(#{seed}) = #{expected}") { expect(DJSP.oneshot seed, base: base).to eq expected }
        end
      end
    end
  end

  context "sequence" do
    it "should only take arithmetic sequences" do
      expect { DJSP.sequence ?a..?z }.to raise_error TypeError
      expect { DJSP.sequence nil.. }.to raise_error TypeError
    end
    it "should only accept known optimizations" do
      expect { DJSP.sequence optimize: :nonexistent }.to raise_error ArgumentError
      expect { DJSP.sequence optimize: "nonexistent" }.to raise_error ArgumentError
      expect { DJSP.sequence optimize: %q(non existent) }.to raise_error ArgumentError
    end
    it "should fail on unimplemented optimizations" do
      expect { DJSP.sequence base: 3, optimize: :root }.to raise_error RangeError
    end
    it("should give a hash") { expect(DJSP.sequence 1..1).to eq ({}) }

    context "2-ary" do
      SEQUENCE_TESTS[2].each do |over, expected|
        it("A(#{over}) = #{expected}") { expect(DJSP.sequence over).to eq expected }
      end
    end
  end
end
