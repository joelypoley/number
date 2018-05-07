#include "integer.h"

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "gtest/gtest.h"
#pragma clang diagnostic pop

const uint32_t max_int32_t = std::numeric_limits<int32_t>::max();
const uint32_t min_int32_t = std::numeric_limits<int32_t>::min();
const uint32_t max_uint32_t = std::numeric_limits<uint32_t>::max();

TEST(IntTest, ConstructorFromInt) {
  const Int a{0};
  const std::vector<uint32_t> a_digits{0};
  EXPECT_EQ(a.sign(), 1);
  EXPECT_EQ(a.get_digits(), a_digits);

  const Int b{1};
  const std::vector<uint32_t> b_digits{1};
  EXPECT_EQ(b.sign(), 1);
  EXPECT_EQ(b.get_digits(), b_digits);

  const Int c{-1};
  const std::vector<uint32_t> c_digits{1};
  EXPECT_EQ(c.sign(), -1);
  EXPECT_EQ(c.get_digits(), c_digits);

  const Int d{100};
  const std::vector<uint32_t> d_digits{100};
  EXPECT_EQ(d.sign(), 1);
  EXPECT_EQ(d.get_digits(), d_digits);

  const Int e{-100};
  const std::vector<uint32_t> e_digits{100};
  EXPECT_EQ(e.sign(), -1);
  EXPECT_EQ(e.get_digits(), e_digits);

  const Int f{max_int32_t};
  EXPECT_EQ(f.sign(), 1);
  const std::vector<uint32_t> f_digits{max_int32_t};
  EXPECT_EQ(f.get_digits(), f_digits);

  Int g(min_int32_t);
  EXPECT_EQ(g.sign(), -1);
  std::vector<uint32_t> g_digits{0x80000000U};
  EXPECT_EQ(g.get_digits(), g_digits);
}

TEST(IntTest, StringConstructor) {
  const Int a{"0"};
  const std::vector<uint32_t> a_digits{0};
  EXPECT_EQ(a.sign(), 1);
  EXPECT_EQ(a.get_digits(), a_digits);

  const Int b{"-0"};
  const std::vector<uint32_t> b_digits{0};
  EXPECT_EQ(b.sign(), 1);
  EXPECT_EQ(b.get_digits(), b_digits);

  const Int c{"100"};
  const std::vector<uint32_t> c_digits{100};
  EXPECT_EQ(c.sign(), 1);
  EXPECT_EQ(c.get_digits(), c_digits);

  const Int d{"-100"};
  const std::vector<uint32_t> d_digits{100};
  EXPECT_EQ(d.sign(), -1);
  EXPECT_EQ(d.get_digits(), d_digits);

  const Int e{"4294967295"};
  const std::vector<uint32_t> e_digits{max_uint32_t};
  EXPECT_EQ(e.sign(), 1);
  EXPECT_EQ(e.get_digits(), e_digits);

  const Int f{"-4294967295"};
  const std::vector<uint32_t> f_digits{max_uint32_t};
  EXPECT_EQ(f.sign(), -1);
  EXPECT_EQ(f.get_digits(), f_digits);

  const Int g{"4294967296"};
  const std::vector<uint32_t> g_digits{0, 1};
  EXPECT_EQ(g.sign(), 1);
  EXPECT_EQ(g.get_digits(), g_digits);

  const Int h{"-4294967296"};
  const std::vector<uint32_t> h_digits{0, 1};
  EXPECT_EQ(h.sign(), -1);
  EXPECT_EQ(h.get_digits(), h_digits);

  const Int i{
      "26959946667150639794667015087019630673637144422540572481103610249215"};
  const std::vector<uint32_t> i_digits{max_uint32_t, max_uint32_t, max_uint32_t,
                                       max_uint32_t, max_uint32_t, max_uint32_t,
                                       max_uint32_t};
  EXPECT_EQ(i.sign(), 1);
  EXPECT_EQ(i.get_digits(), i_digits);

  const Int j{
      "-26959946667150639794667015087019630673637144422540572481103610249215"};
  const std::vector<uint32_t> j_digits{max_uint32_t, max_uint32_t, max_uint32_t,
                                       max_uint32_t, max_uint32_t, max_uint32_t,
                                       max_uint32_t};
  EXPECT_EQ(j.sign(), -1);
  EXPECT_EQ(j.get_digits(), j_digits);

  const Int k{"18446744073709551616"};
  const std::vector<uint32_t> k_digits{0, 0, 1};
  EXPECT_EQ(k.sign(), 1);
  EXPECT_EQ(k.get_digits(), k_digits);

  const Int l{"-18446744073709551616"};
  const std::vector<uint32_t> l_digits{0, 0, 1};
  EXPECT_EQ(l.sign(), -1);
  EXPECT_EQ(l.get_digits(), l_digits);

  const Int m{"79228162514264337593543950336"};
  const std::vector<uint32_t> m_digits{0, 0, 0, 1};
  EXPECT_EQ(m.sign(), 1);
  EXPECT_EQ(m.get_digits(), m_digits);

  const Int n{"-79228162514264337593543950336"};
  const std::vector<uint32_t> n_digits{0, 0, 0, 1};
  EXPECT_EQ(n.sign(), -1);
  EXPECT_EQ(n.get_digits(), n_digits);
}

TEST(IntTest, Comparison) {
  const Int negative_hundred{-100};
  const Int negative_one{-1};
  const Int zero{0};
  const Int one{1};
  const Int hundred{100};

  EXPECT_TRUE(negative_hundred < negative_one);
  EXPECT_TRUE(-100 < negative_one);
  EXPECT_TRUE(negative_hundred < -1);
  EXPECT_TRUE(negative_one < zero);
  EXPECT_TRUE(zero < one);
  EXPECT_TRUE(one < hundred);
  EXPECT_FALSE(hundred < one);
  EXPECT_FALSE(one < zero);
  EXPECT_FALSE(zero < negative_one);
  EXPECT_FALSE(negative_one < negative_hundred);

  const Int a{
      "-26959946667150639794667015087019630673637144422540572481103610249215"};
  const Int b{"-4294967296"};
  const Int c{"-4294967295"};
  const Int d{"4294967295"};
  const Int e{"4294967296"};
  const Int f{
      "26959946667150639794667015087019630673637144422540572481103610249215"};

  EXPECT_TRUE(a < b);
  EXPECT_TRUE(b < c);
  EXPECT_TRUE(c < 0);
  EXPECT_TRUE(c < 1);
  EXPECT_TRUE(c < d);
  EXPECT_TRUE(0 < d);
  EXPECT_TRUE(1 < d);
  EXPECT_TRUE(d < e);

  EXPECT_FALSE(b < a);
  EXPECT_FALSE(c < b);
  EXPECT_FALSE(0 < c);
  EXPECT_FALSE(1 < c);
  EXPECT_FALSE(d < c);
  EXPECT_FALSE(d < 0);
  EXPECT_FALSE(d < 1);
  EXPECT_FALSE(e < d);

  Int g{"55340232234013556739"};
  Int h{"73786976303428141057"};
  EXPECT_TRUE(g < h);
  EXPECT_FALSE(h < g);
}

TEST(IntTest, Equality) {
  const Int hundred{100};
  const Int another_hundred{100};
  const Int big{
      "-26959946667150639794667015087019630673637144422540572481103610249215"};
  const Int another_big{
      "-26959946667150639794667015087019630673637144422540572481103610249215"};

  EXPECT_TRUE(hundred == another_hundred);
  EXPECT_TRUE(big == another_big);
  EXPECT_FALSE(hundred == big);
}

TEST(IntTest, UnaryMinus) {
  const Int negative_two{-2};
  const Int negative_one{-1};
  const Int zero{0};
  const Int one{1};
  const Int two{2};
  const Int big{
      "26959946667150639794667015087019630673637144422540572481103610249215"};
  const Int negative_big{
      "-26959946667150639794667015087019630673637144422540572481103610249215"};

  EXPECT_EQ(-negative_two, two);
  EXPECT_EQ(-two, negative_two);
  EXPECT_EQ(-negative_one, one);
  EXPECT_EQ(-one, negative_one);
  EXPECT_EQ(-zero, zero);
  EXPECT_EQ(-big, negative_big);
  EXPECT_EQ(-negative_big, big);
}

TEST(IntTest, Carry) {
  std::pair<uint32_t, uint32_t> res{0, 0};
  EXPECT_EQ(add_with_carry(0, 0, 0), res);

  res = std::make_pair(3, 0);
  EXPECT_EQ(add_with_carry(1, 1, 1), res);

  res = std::make_pair(max_uint32_t, 0);
  EXPECT_EQ(add_with_carry(max_uint32_t - 1, 1, 0), res);

  res = std::make_pair(max_uint32_t, 0);
  EXPECT_EQ(add_with_carry(max_uint32_t - 1, 0, 1), res);

  res = std::make_pair(0, 1);
  EXPECT_EQ(add_with_carry(max_uint32_t, 1, 0), res);

  res = std::make_pair(0, 1);
  EXPECT_EQ(add_with_carry(max_uint32_t, 0, 1), res);

  res = std::make_pair(10, 1);
  EXPECT_EQ(add_with_carry(max_uint32_t, 11, 0), res);

  res = std::make_pair(4, 1);
  EXPECT_EQ(add_with_carry(max_uint32_t, 5, 0), res);

  res = std::make_pair(0, 1);
  EXPECT_EQ(add_with_carry(max_uint32_t, 0, 1), res);
}

TEST(IntTest, Add) {
  const Int negative_two{-2};
  const Int negative_one{-1};
  const Int zero{0};
  const Int one{1};
  const Int two{2};
  EXPECT_EQ(negative_two + one, negative_one);
  EXPECT_EQ(negative_two + two, zero);
  EXPECT_EQ(zero + zero, zero);
  EXPECT_EQ(one + one, two);

  const Int a{"6277101735386680763835789423207666416120802188576398770185"};
  const Int minus_a{
      "-6277101735386680763835789423207666416120802188576398770185"};
  const Int b{"6277101735386680763835789423207666416120802188576398770190"};
  const Int minus_b{
      "-6277101735386680763835789423207666416120802188576398770190"};
  const Int c{"12554203470773361527671578846415332832241604377152797540375"};
  const Int minus_c{
      "-12554203470773361527671578846415332832241604377152797540375"};

  EXPECT_EQ(a + 5, b);
  EXPECT_EQ(a + b, c);
  EXPECT_EQ(a + minus_b, -5);
  EXPECT_EQ(minus_b + a, -5);
  EXPECT_EQ(a + minus_c, minus_b);

  const Int d{"4294967295"};
  const Int e{"4294967296"};
  EXPECT_EQ(e + negative_one, d);
}

TEST(IntTest, Subtraction) {
  Int negative_two{-2};
  Int negative_one{-1};
  Int zero{0};
  Int one{1};
  Int two{2};

  EXPECT_EQ(one - 1, zero);
  EXPECT_EQ(zero - zero, zero);

  const Int d{"4294967295"};
  const Int e{"4294967296"};
  EXPECT_EQ(e - 1, d);

  const Int a{"6277101735386680763835789423207666416120802188576398770185"};
  const Int minus_a{
      "-6277101735386680763835789423207666416120802188576398770185"};
  const Int b{"6277101735386680763835789423207666416120802188576398770190"};
  const Int minus_b{
      "-6277101735386680763835789423207666416120802188576398770190"};
  const Int c{"12554203470773361527671578846415332832241604377152797540375"};
  const Int minus_c{
      "-12554203470773361527671578846415332832241604377152797540375"};

  EXPECT_EQ(a - b, -5);
  EXPECT_EQ(b - a, 5);
  EXPECT_EQ(a - c, -b);
  EXPECT_EQ(c - a, b);
}

TEST(IntTest, Multiply) {
  Int negative_two{-2};
  Int negative_one{-1};
  Int zero{0};
  Int one{1};
  Int two{2};

  EXPECT_EQ(negative_two * 0, 0);
  EXPECT_EQ(one * 1, 1);
  EXPECT_EQ(zero * zero, zero);
  EXPECT_EQ(negative_one * -1, 1);
  EXPECT_EQ(2 * negative_two, -4);

  const Int d{"4294967295"};
  const Int e{"4294967296"};
  const Int f{"18446744069414584320"};
  EXPECT_EQ(e * d, f);

  const Int a{"6277101735386680763835789423207666416120802188576398770185"};
  const Int b{"6277101735386680763835789423207666416120802188576398770190"};
  const Int c{"12554203470773361527671578846415332832241604377152797540375"};
  const Int g{
      "394020061963944792122790401001436138053113234494253580989485202304809975"
      "16338667371973139355530553882773662438785150"};

  EXPECT_EQ(a * b, g);
  EXPECT_EQ(-a * b, -g);
  EXPECT_EQ(a * -b, -g);
  EXPECT_EQ(-a * -b, g);
}

TEST(IntTest, Divide) {
  Int negative_two{-2};
  Int negative_one{-1};
  Int zero{0};
  Int one{1};
  Int two{2};
  Int twelve{12};
  Int three{3};
  Int eleven{11};

  EXPECT_EQ(two / 1, 2);
  EXPECT_EQ(two / two, 1);
  EXPECT_EQ(twelve / three, 4);
  EXPECT_EQ(eleven / three, 3);
  EXPECT_EQ(-two / 1, -2);
  EXPECT_EQ(-two / -two, 1);
  EXPECT_EQ(twelve / -three, -4);
  EXPECT_EQ(-eleven / -three, 3);

  const Int d{"4294967295"};
  const Int e{"4294967296"};
  const Int f{"18446744069414584320"};
  EXPECT_EQ(f / e, d);
  EXPECT_EQ(-f / e, -d);
  EXPECT_EQ(f / -e, -d);
  EXPECT_EQ(-f / -e, d);

  const Int h{"73786976303428141057"};
  const Int i{"18446744078004518913"};
  EXPECT_EQ(h / i, 3);
  const Int a{"6277101735386680763835789423207666416120802188576398770185"};
  const Int b{"6277101735386680763835789423207666416120802188576398770190"};
  const Int c{"12554203470773361527671578846415332832241604377152797540375"};
  const Int g{
      "394020061963944792122790401001436138053113234494253580989485202304809975"
      "16338667371973139355530553882773662438785150"};
  EXPECT_EQ(g / a, b);
  EXPECT_EQ((g + 1) / a, b);
  EXPECT_EQ((g - 1) / a, b - 1);

  const Int j{
      "107150860718626732094842504906000181056140481170553360744375038837035105"
      "112493612249319837881569585812759467291755314682518714528569231404359845"
      "775746985748039345677748242309854210746050623711418779541821530464749835"
      "819412673987675591655439460770629145711964776865421676604298316526243868"
      "37205668069673"};
  const Int k{"1267650600228229401496703205653"};
  const Int l{
      "845271249817064394163743655681722570639383758265782540842118790220758953"
      "646318510379622854683943520485643104676182742446810227956018905102048857"
      "651352142905807188249167683186261999596305061323296344567902698683601207"
      "5342454965044815321632851512196247686534109870790600459"};

  EXPECT_EQ(j / k, l);
  EXPECT_EQ(-j / k, -l);
  EXPECT_EQ(j / -k, -l);
  EXPECT_EQ(-j / -k, l);
}
